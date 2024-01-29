set_project("micro_sockets")
set_version("0.1.0")
set_languages("c11")

add_rules("mode.debug", "mode.release")

target("micro_sockets")
  set_default(true)
  set_kind("headeronly")
  add_headerfiles("include/(micro_sockets/*.h)")
  add_rules("utils.install.cmake_importfiles")
  add_rules("utils.install.pkgconfig_importfiles")

target("examples/tcp_client")
  set_enabled(false)
  set_kind("binary")
  add_files("examples/tcp_client.c")
  add_deps("micro_sockets")

target("examples/tcp_server")
  set_enabled(false)
  set_kind("binary")
  add_files("examples/tcp_server.c")
  add_includedirs("include")
