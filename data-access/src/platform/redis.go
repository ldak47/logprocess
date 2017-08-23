package main

import (
    "fmt"
    "encoding/json"
    "bytes"
    "github.com/garyburd/redigo/redis"
)

func GetDataAccessers () ([]string) {
    addrs := []string {
        "127.0.0.1:30000",
        "127.0.0.1:30001",
        "127.0.0.1:30002",
        "127.0.0.1:30003",
        "127.0.0.1:30004",
        "127.0.0.1:30005",
        "127.0.0.1:30006",
        "127.0.0.1:30007",
        "127.0.0.1:30008",
        "127.0.0.1:30009",
        "127.0.0.1:30010",
        "127.0.0.1:30011",
        "127.0.0.1:30012",
        "127.0.0.1:30013",
        "127.0.0.1:30014",
        "127.0.0.1:30015",
        "127.0.0.1:30016",
        "127.0.0.1:30017",
        "127.0.0.1:30018",
        "127.0.0.1:30019",
    }

    return addrs
}

type Health struct {
    Meminfo string
    Cpuinfo string
    Threadinfo string
}

func GetHealth () {
    conn, err := redis.Dial("tcp", "10.99.196.105:6379")
    if err == nil {
        defer conn.Close()
        addrs := GetDataAccessers()

        for i := 0; i < len(addrs); i++ {
            var info Health
            info.Meminfo = "102400"
            info.Cpuinfo = "1.3"
            info.Threadinfo = "51"
            jsonarg, err := json.Marshal(info)
            if err == nil {
                postbody := bytes.NewBuffer([]byte(jsonarg))
                health, err := conn.Do("set", addrs[i], postbody)
                if err == nil {
                    fmt.Println(health)
                }
            }
        }
        
        for i := 0; i < len(addrs); i++ {
            var healthary interface{}
            health, err := redis.String(conn.Do("get", addrs[i]))
            if err == nil {
                json.Unmarshal([]byte(health), &healthary)
                m := healthary.(map[string]interface{})
                meminfo := m["Meminfo"]
                cpuinfo := m["Cpuinfo"]
                threadinfo := m["Threadinfo"]
                fmt.Println(meminfo)
                fmt.Println(cpuinfo)
                fmt.Println(threadinfo)
            }
        }
    }
}

func main () {
    GetHealth()
}
