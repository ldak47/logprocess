package main

import (
    "fmt"
    "github.com/c4pt0r/ini"
)
    
var conf = ini.NewConf("/home/work/data-access/src/platform/src/logplatform/conf/app.conf")
var dataaccess_addrs = conf.String("platform", "dataaccess_addrs", "")

func main () {
    conf.Parse()
    fmt.Println(*dataaccess_addrs)
    return
}
