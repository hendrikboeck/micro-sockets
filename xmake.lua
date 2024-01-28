set_languages("c11")

add_rules("mode.debug", "mode.release")

target("examples/tcp_client")
  set_kind("binary")
  add_files("examples/tcp_client.c")
  add_includedirs("include")

target("examples/tcp_server")
  set_kind("binary")
  add_files("examples/tcp_server.c")
  add_includedirs("include")
