set_project("micro-sockets")
set_version("0.1.0")
set_languages("c11")

add_rules("mode.debug", "mode.release")

add_repositories("hendrikboeck-ppa https://github.com/hendrikboeck/xmake-ppa.git main")
add_requires("ccms")

add_rules("plugin.compile_commands.autoupdate", { outputdir = "." })
target("micro-sockets")
  set_default(true)
  set_kind("headeronly")
  add_packages("ccms", { public = true })
  add_headerfiles("include/(micro-sockets/*.h)", { public = true })
  add_includedirs("include", { public = true })
  add_rules("utils.install.cmake_importfiles")
  add_rules("utils.install.pkgconfig_importfiles")

target("examples/tcp_client")
  set_enabled(true)
  set_kind("binary")
  add_files("examples/tcp_client.c")
  add_deps("micro-sockets")

target("examples/tcp_server")
  set_enabled(true)
  set_kind("binary")
  add_files("examples/tcp_server.c")
  add_deps("micro-sockets")
