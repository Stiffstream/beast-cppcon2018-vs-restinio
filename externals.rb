MxxRu::arch_externals :restinio do |e|
  e.url 'https://github.com/Stiffstream/restinio/releases/download/v.0.6.0/restinio-0.6.0-full.tar.bz2'

  # RESTinio and its dependencies.
  e.map_dir 'dev/asio' => 'dev'
  e.map_dir 'dev/asio_mxxru' => 'dev'
  e.map_dir 'dev/nodejs' => 'dev'
  e.map_dir 'dev/fmt' => 'dev'
  e.map_dir 'dev/fmt_mxxru' => 'dev'
  e.map_dir 'dev/restinio' => 'dev'
  e.map_dir 'dev/cmake' => 'dev'
end

