{
  "myapp" : {
    "root" : "/dsp"
  },
  "service" : {
    "api" : "fastcgi",
    "worker_processes": 1,
    "worker_threads": 100,
    "socket" : "/tmp/dsp-fcgi.socket",
    "disable_xpowered_by" : true,
    "generate_http_headers" : false
  },
  "http" : {
    "script_names" : [ "/dsp" ]
  }
}
