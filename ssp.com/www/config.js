{
  "myapp" : {
    "root" : "/ttj"
  },
  "service" : {
    "api" : "fastcgi",
    "worker_processes": 1,
    "worker_threads": 100,
    "socket" : "/tmp/ttj-fcgi.socket",
    "disable_xpowered_by" : true,
    "generate_http_headers" : false
  },
  "http" : {
    "script_names" : [ "/ttj" ]
  }
}
