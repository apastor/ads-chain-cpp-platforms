{
  "myapp" : {
    "root" : "/mostachio"
  },
  "views" : {
    "paths" : [ "." ],
    "skins" : [ "my_skin" ]
  },
  "service" : {
    "api" : "fastcgi",
    "worker_processes": 1,
    "worker_threads": 100,
    "socket" : "/tmp/mostachio-fcgi.socket",
    "disable_xpowered_by" : true,
    "generate_http_headers": false
  },
  "http" : {
    "script_names" : [ "/mostachio" ]
  }
}
