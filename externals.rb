MxxRu::arch_externals :restinio do |e|
  e.url 'https://bitbucket.org/sobjectizerteam/restinio-0.4/downloads/restinio-0.4.8-full.tar.bz2'

  # RESTinio and its dependencies.
  e.map_dir 'dev/asio' => 'dev'
  e.map_dir 'dev/asio_mxxru' => 'dev'
  e.map_dir 'dev/nodejs' => 'dev'
  e.map_dir 'dev/fmt' => 'dev'
  e.map_dir 'dev/fmt_mxxru' => 'dev'
  e.map_dir 'dev/restinio' => 'dev'
end

