#include <restinio/all.hpp>
#include <restinio/websocket/websocket.hpp>

#include <iostream>
#include <functional>

namespace restinio_ws = restinio::websocket::basic;

// This traits will be used by RESTinio server.
// This type also necessary for websocket's upgrade function.
using server_traits_t = restinio::default_single_thread_traits_t;

// Type of active websockets registry.
// Unique websocket ID is used as key.
using ws_registry_t = std::map< std::uint64_t, restinio_ws::ws_handle_t >;

// Attempt to detect type of file from file's extension.
//
// NOTE: std::string is returned, not a string_view.
// It is because the result of mime_type() is going to append_header() method
// and this method accepts only std::string in the current version of RESTinio.
std::string mime_type( restinio::string_view_t path )
{
	using restinio::caseless_cmp;

	auto const ext = [&path]() -> restinio::string_view_t {
		auto const pos = path.rfind(".");
		if( pos == restinio::string_view_t::npos ) return {};
		return path.substr(pos);
	}();

	if( caseless_cmp(ext, ".htm") )  return "text/html";
	if( caseless_cmp(ext, ".html") ) return "text/html";
	if( caseless_cmp(ext, ".php") )  return "text/html";
	if( caseless_cmp(ext, ".css") )  return "text/css";
	if( caseless_cmp(ext, ".txt") )  return "text/plain";
	if( caseless_cmp(ext, ".js") )   return "application/javascript";
	if( caseless_cmp(ext, ".json") ) return "application/json";
	if( caseless_cmp(ext, ".xml") )  return "application/xml";
	if( caseless_cmp(ext, ".swf") )  return "application/x-shockwave-flash";
	if( caseless_cmp(ext, ".flv") )  return "video/x-flv";
	if( caseless_cmp(ext, ".png") )  return "image/png";
	if( caseless_cmp(ext, ".jpe") )  return "image/jpeg";
	if( caseless_cmp(ext, ".jpeg") ) return "image/jpeg";
	if( caseless_cmp(ext, ".jpg") )  return "image/jpeg";
	if( caseless_cmp(ext, ".gif") )  return "image/gif";
	if( caseless_cmp(ext, ".bmp") )  return "image/bmp";
	if( caseless_cmp(ext, ".ico") )  return "image/vnd.microsoft.icon";
	if( caseless_cmp(ext, ".tiff") ) return "image/tiff";
	if( caseless_cmp(ext, ".tif") )  return "image/tiff";
	if( caseless_cmp(ext, ".svg") )  return "image/svg+xml";
	if( caseless_cmp(ext, ".svgz") ) return "image/svg+xml";
	return "application/text";
}

// Create an actual root_dir value.
auto make_root_dir_value( std::string root_dir )
{
	if( root_dir.empty() )
		root_dir = ".";
	else if( root_dir.back() == '/' || root_dir.back() == '\\' )
		root_dir.pop_back();

	return root_dir;
}

// Helper function for creation of negative response for a request.
auto mk_negative_resp(
	restinio::request_handle_t & req,
	restinio::http_status_line_t status_line )
{
	return req->create_response( std::move(status_line) )
			.append_header_date_field()
			.connection_close()
			.done();
}

// A handler for websocket-related events.
// This handler will be installed for every new upgraded connection.
void ws_handler(
	ws_registry_t & registry,
	restinio_ws::ws_handle_t wsh, 
	restinio_ws::message_handle_t m )
{
	// Closing frame should be treated a special way.
	if( m->opcode() == restinio_ws::opcode_t::connection_close_frame )
	{
		// This connection must be removed from registry.
		// New messages won't go to that connection anymore.
		registry.erase( wsh->connection_id() );

		// Reply to close_frame if this frame is received from a client.
		wsh->send_message( *m );
		// Close websocket.
		// It is necessary if close_frame is received from a client.
		wsh->shutdown();
	}
	else
	{
		// We don't distinguish all other types of frames because the
		// original example doesn't do it.
		//
		// Just broadcast the current message for all connected websockets.
		for( auto & ws_handler: registry )
		{
			ws_handler.second->send_message( *m );
		}
	}
};

// A handler of incoming request to be used for upgrade requests.
auto handle_websocket_upgrade(
	ws_registry_t & registry,
	restinio::request_handle_t & req )
{
	using namespace std::placeholders;

	// Do upgrade and get ws_handle for new websocket.
	auto wsh = restinio_ws::upgrade<server_traits_t>(
			*req,
			restinio_ws::activation_t::immediate,
			std::bind( ws_handler, std::ref(registry), _1, _2 ) );

	registry.emplace( wsh->connection_id(), wsh );

	return restinio::request_accepted();
}

// Helper function for returning the whole file as request's response.
auto serve_requested_file(
	restinio::request_handle_t & req,
	const restinio::string_view_t & target,
	const std::string & path )
{
	try
	{
		// RESTinio's sendfile functionality will be used here.
		auto sf = restinio::sendfile( path );

		if( restinio::http_method_head() == req->header().method() )
			// Handle HTTP HEAD request. Only file length should be returned.
			return req->create_response< restinio::user_controlled_output_t >()
					.append_header( restinio::http_field::server, "RESTinio" )
					.append_header_date_field()
					.append_header( restinio::http_field::content_type, mime_type( path ) )
					.set_content_length( static_cast<std::size_t>(
								sf.meta().file_total_size()) )
					.done();

		else
			// Handle HTTP GET request. Return the whole file.
			return req->create_response()
					.append_header( restinio::http_field::server, "RESTinio" )
					.append_header_date_field()
					.append_header( restinio::http_field::content_type, mime_type( path ) )
					.set_body( std::move( sf ) )
					.done();

	}
	catch( const std::exception & )
	{
		return mk_negative_resp(
				req,
				restinio::http_status_line_t{
						restinio::status_code::not_found,
						std::string{ target.data(), target.size() } } );
	}
}

// A handler for incoming requests which are not upgrade requests.
auto handle_ordinary_request(
	const std::string & server_root_dir,
	restinio::request_handle_t & req )
{
	// Only HTTP GET and HEAD are supported.
	if( restinio::http_method_get() != req->header().method() &&
			restinio::http_method_head() != req->header().method() )
		return mk_negative_resp(
				req, restinio::status_method_not_allowed() );

	// A valid target name is expected.
	const auto & target = req->header().path();
	if( target.empty() ||
			target[0] != '/' ||
			std::string::npos != target.find( ".." ) )
		return mk_negative_resp(
				req, restinio::status_bad_request() );

	// Make the actual path to the file requested.
	auto path = server_root_dir;
	// Use std::string::append because there isn't
	// operator+(std::string, string_view) on old compilers.
	path.append( target.data(), target.size() );
	if( path.back() == '/' )
		path += "index.html";

	return serve_requested_file( req, target, path );
}

// Creates a request handler.
auto make_server_handler( ws_registry_t & registry, std::string root_dir )
{
	return [&registry,
			 server_root_dir = make_root_dir_value(std::move(root_dir))]
		(auto req)
		{
			try
			{
				if( restinio::http_connection_header_t::upgrade
						== req->header().connection() )
					return handle_websocket_upgrade( registry, req );
				else
					return handle_ordinary_request( server_root_dir, req );
			}
			catch( const std::exception & x )
			{
				std::cerr << "Exception caught: " << x.what() << std::endl;
				return restinio::request_rejected();
			}
		};
}

int main( int argc, char const *argv[] )
{
	if( 4 != argc )
	{
		std::cerr << "Usage: websocket_chat_app <addr> <port> <doc_root>"
				<< std::endl;
		return 2;
	}

	// An instance of websocket registry.
	// The reference to that instance will be passed to all websocket handlers.
	ws_registry_t registry;

	// Run RESTinio server.
	// Server will be stopped on SIGINT.
	restinio::run(
		restinio::on_this_thread<server_traits_t>()
			.address( argv[1] )
			.port( static_cast<unsigned int>(std::atoi( argv[2] )) )
			.request_handler( make_server_handler( registry, argv[3] ) )
			// This is a special moment: websocket registry must be
			// manually cleaned up before the destruction of underlying
			// server infrastructure.
			// Because of that we specify a cleanup function for server.
			.cleanup_func( [&]{ registry.clear(); } ) );

	return 0;
}

