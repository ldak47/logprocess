package controllers

import (
    //"fmt"
    "strings"
    "strconv"
    "encoding/json"
    "github.com/garyburd/redigo/redis"
    "github.com/astaxie/beego"
)

type RelaxController struct {
    beego.Controller
}

func (c *RelaxController) Get () {
    conf.Parse()
    conn, err := redis.Dial("tcp", *redis_addr)
    if err == nil {
        addr_array := strings.FieldsFunc(*dataaccess_addrs, split)
        var freenode string = ""
        var min_mem int = 1024 * 1000 * 10
        var min_cpu float64 = 100
        for _, addr := range addr_array {
            health, err := redis.String(conn.Do("get", addr))
            if err != nil {
                continue
            } else {
                var m map[string]string = make(map[string]string)
                json.Unmarshal([]byte(health), &m)
                meminfo,_ := strconv.Atoi(m["meminfo"])
                cpuinfo,_ := strconv.ParseFloat(m["Cpuinfo"], 64)
                if meminfo < min_mem {
                    min_mem = meminfo
                    min_cpu = cpuinfo
                    freenode = addr
                } else if meminfo == min_mem {
                    if cpuinfo < min_cpu {
                        min_mem = meminfo
                        min_cpu = cpuinfo
                        freenode = addr
                    }
                }
            }
        }
        c.Data["json"] = map[string]interface{}{"success":0,"message":freenode}
    } else {
        c.Data["json"] = map[string]interface{}{"success":-1,"message":"connect redis fail"}
    }

    //fmt.Println(c.Data["json"])
    c.ServeJSON()
    return
}
