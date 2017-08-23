package main

import (
    "fmt"
    "log"
    "encoding/json"
    "bytes"
    "strings"
    "./transmit"
)

type GetRequest struct {
    Application bool
}

type SetRequest struct {
    Op bool
    Srcid string
}

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

func SetLogFilterConfig (addr string, op bool, srcid string, resultChan chan transmit.RemoteResult) {
    var module string = "manage.ManageService."
    var method string = "SetLogFilterConfig"
    var url string = strings.Join([]string{"http://", addr, "/", module, method}, "")

    var postarg SetRequest
    postarg.Op = op
    postarg.Srcid = srcid
    jsonarg, err := json.Marshal(postarg)
    if err != nil {
        var errmsg = "json Marshal fail: "
        log.Println(errmsg, err)
        fmt.Println(errmsg, err)
        resultChan <- transmit.RemoteResult{Addr: addr, Result: errmsg}
        return
    }
    postbody := bytes.NewBuffer([]byte(jsonarg))
    transmit.Transmit(postbody, url, addr, resultChan)
}

func GetLogFilterConfig (addr string, resultChan chan transmit.RemoteResult) {
    var module string = "manage.ManageService."
    var method string = "GetLogFilterConfig"
    var url string = strings.Join([]string{"http://", addr, "/", module, method}, "")

    var postarg GetRequest
    postarg.Application = true
    jsonarg, err := json.Marshal(postarg)
    if err != nil {
        var errmsg = "json Marshal fail: "
        log.Println(errmsg, err)
        fmt.Println(errmsg, err)
        resultChan <- transmit.RemoteResult{Addr: addr, Result: errmsg}
        return
    }
    postbody := bytes.NewBuffer([]byte(jsonarg))
    transmit.Transmit(postbody, url, addr, resultChan)
}

func MultiGet () ([]transmit.RemoteResult) {
    addrs := GetDataAccessers()
    resultChan := make(chan transmit.RemoteResult, len(addrs))
    defer close(resultChan)
    var result []transmit.RemoteResult

    for _, addr := range addrs {
        go GetLogFilterConfig(addr, resultChan)
    }

    for i := 0; i < len(addrs); i++ {
        res := <-resultChan
        result = append(result, res)
    }

    return result
}

func MultiSet (op bool, srcid string) ([]transmit.RemoteResult) {
    addrs := GetDataAccessers()
    resultChan := make(chan transmit.RemoteResult, len(addrs))
    defer close(resultChan)
    var result []transmit.RemoteResult

    for _, addr := range addrs {
        go SetLogFilterConfig(addr, op, srcid, resultChan)
    }

    for i := 0; i < len(addrs); i++ {
        res := <-resultChan
        result = append(result, res)
    }

    return result
}

func main () {
    //read
    result := MultiGet()
    for _, res := range result {
        fmt.Println(res)
    }

    //add 5555
    var op bool = true
    var srcid string = "5555"
    result = MultiSet(op, srcid)
    for _, res := range result {
        fmt.Println(res)
    }
    //read
    result = MultiGet()
    for _, res := range result {
        fmt.Println(res)
    }

    //del 4178
    op = false
    srcid = "4178"
    result = MultiSet(op, srcid)
    for _, res := range result {
        fmt.Println(res)
    }
    //read
    result = MultiGet()
    for _, res := range result {
        fmt.Println(res)
    }
}
