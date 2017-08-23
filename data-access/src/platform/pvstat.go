package main

import (
    "fmt"
    "log"
    "encoding/json"
    "bytes"
    "strings"
    "./transmit"
)

func GetDataAccessers () ([]string) {
    addrs := []string {
        "10.99.196.105:8000",
        "10.99.196.105:8001",
        "10.99.196.105:8002",
        "10.99.196.105:8003",
        "10.99.196.105:8004",
        "10.120.208.11:8000",
        "10.120.208.11:8001",
        "10.120.208.11:8002",
        "10.120.208.11:8003",
        "10.120.208.11:8004",
        "10.120.208.11:8005",
        "10.120.208.11:8006",
        "10.120.208.11:8007",
        "10.120.208.11:8008",
        "10.120.208.11:8009",
        "10.120.208.11:8010",
        "10.120.208.11:8011",
        "10.120.208.11:8012",
        "10.120.208.11:8013",
        "10.120.208.11:8014",
    }

    return addrs
}

type GetRequest struct {
    Application string
    Startime int64
    Endtime int64
}

type SetRequest struct {
    Application string
    Srcid string
}

func AddLogTransmitStat (addr string, application string, srcid string, resultChan chan transmit.RemoteResult) {
    var module string = "manage.ManageService."
    var method string = "AddLogTransmitStat"
    var url string = strings.Join([]string{"http://", addr, "/", module, method}, "")

    var postarg SetRequest
    postarg.Application = application
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

func GetLogTransmitStat (addr string, application string, start int64, end int64, resultChan chan transmit.RemoteResult) {
    var module string = "manage.ManageService."
    var method string = "GetLogTransmitStat"
    var url string = strings.Join([]string{"http://", addr, "/", module, method}, "")

    var postarg GetRequest
    postarg.Application = application
    postarg.Startime = start
    postarg.Endtime = end

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

func MultiSet (application string, srcid string) ([]transmit.RemoteResult) {
    addrs := GetDataAccessers()
    resultChan := make(chan transmit.RemoteResult, len(addrs))
    defer close(resultChan)
    var result []transmit.RemoteResult

    for _, addr := range addrs {
        go AddLogTransmitStat(addr, application, srcid, resultChan)
    }

    for i := 0; i < len(addrs); i++ {
        res := <-resultChan
        result = append(result, res)
    }

    return result
}

func MultiGet (application string, start int64, end int64) ([]transmit.RemoteResult) {
    addrs := GetDataAccessers()
    resultChan := make(chan transmit.RemoteResult, len(addrs))
    defer close(resultChan)
    var result []transmit.RemoteResult

    for _, addr := range addrs {
        go GetLogTransmitStat(addr, application, start, end, resultChan)
    }

    for i := 0; i < len(addrs); i++ {
        res := <-resultChan
        result = append(result, res)
    }

    return result
}

func main () {
    //read
    var application string = "weather"
    var start int64 = 1501574400
    var end int64 = 1503045060
    result := MultiGet(application, start, end)
    for _, res := range result {
        fmt.Println(res)
    }
    return

    //add new srcid on old application
    var srcid string = "4555"
    result = MultiSet(application, srcid)
    for _, res := range result {
        fmt.Println(res)
    }
    //read
    result = MultiGet(application, start, end)
    for _, res := range result {
        fmt.Println(res)
    }

    //add new srcid on new application
    application = "huilv"
    srcid = "6666"
    result = MultiSet(application, srcid)
    for _, res := range result {
        fmt.Println(res)
    }
    //read
    application = "weather"
    result = MultiGet(application, start, end)
    for _, res := range result {
        fmt.Println(res)
    }

}



