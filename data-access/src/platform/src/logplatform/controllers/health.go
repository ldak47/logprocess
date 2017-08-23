package controllers

import (
    "fmt"
    "strings"
    "github.com/garyburd/redigo/redis"
    "github.com/astaxie/beego"
    "github.com/c4pt0r/ini"
)

var conf = ini.NewConf("/home/work/data-access/src/platform/src/logplatform/conf/app.conf")
var dataaccess_addrs = conf.String("platform", "dataaccess_addrs", "")
var dataprocess_addrs = conf.String("platform", "dataprocess_addrs", "")
var redis_addr = conf.String("platform", "redis_addr", "")

type HealthController struct {
    beego.Controller
}

type Health struct {
    Meminfo uint32
    Cpuinfo float64
    Threadinfo int
}

func split (s rune) bool {
    if s == ',' {
        return true
    } else {
        return false
    }
}

func (c *HealthController) Get () {
    conf.Parse()
    conn, err := redis.Dial("tcp", *redis_addr)
    if err == nil {
        addr_array := strings.FieldsFunc(*dataaccess_addrs, split)
        var healths map[string]string = make(map[string]string)
        for _, addr := range addr_array {
            health, err := redis.String(conn.Do("get", addr))
            if err != nil {
                continue
            } else {
                healths[addr] = health
            }
        }
        c.Data["json"] = healths
    } else {
        c.Data["json"] = map[string]interface{}{"success":-1,"message":"connect redis fail"}
    }

    fmt.Println(c.Data["json"])
    c.ServeJSON()
    return
}
