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

type GetRequest struct {
    Application string
}

type FiledConfig struct {
    Field string
    Regex string
    Istrans bool
}

type SetRequest struct {
    Application string
    Op bool
    Fieldconfigs FiledConfig
}

func SetLogFieldConfig (addr string, application string, op bool, field string, regex string, istrans bool, resultChan chan transmit.RemoteResult) {
    var module string = "manage.ManageService."
    var method string = "SetLogFieldConfig"
    var url string = strings.Join([]string{"http://", addr, "/", module, method}, "")

    var postarg SetRequest
    postarg.Application = application
    postarg.Op = op
    var FiledConfig FiledConfig
    FiledConfig.Field = field
    FiledConfig.Regex = regex
    FiledConfig.Istrans = istrans
    postarg.Fieldconfigs = FiledConfig

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

func GetLogFieldConfig (addr string, application string, resultChan chan transmit.RemoteResult) {
    var module string = "manage.ManageService."
    var method string = "GetLogFieldConfig"
    var url string = strings.Join([]string{"http://", addr, "/", module, method}, "")

    var postarg GetRequest
    postarg.Application = application
    
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

func MultiSet (application string, op bool, field string, regex string, istrans bool) ([]transmit.RemoteResult) {
    addrs := GetDataAccessers()
    resultChan := make(chan transmit.RemoteResult, len(addrs))
    defer close(resultChan)
    var result []transmit.RemoteResult

    for _, addr := range addrs {
        go SetLogFieldConfig(addr, application, op, field, regex, istrans, resultChan)
    }

    for i := 0; i < len(addrs); i++ {
        res := <-resultChan
        result = append(result, res)
    }

    return result
}

func MultiGet (application string) ([]transmit.RemoteResult) {
    addrs := GetDataAccessers()
    resultChan := make(chan transmit.RemoteResult, len(addrs))
    defer close(resultChan)
    var result []transmit.RemoteResult

    for _, addr := range addrs {
        go GetLogFieldConfig(addr, application, resultChan)
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
    result := MultiGet(application)
    for _, res := range result {
        fmt.Println(res)
    }

    //modify
    var op bool = true
    var field string = "qid"
    var regex string = "xxxxx"
    var istrans = true
    result = MultiSet(application, op, field, regex, istrans)
    for _, res := range result {
        fmt.Println(res)
    }
    //read
    result = MultiGet(application)
    for _, res := range result {
        fmt.Println(res)
    }

    //add new one
    field = "qu_hit"
    regex = "yyyyy"
    istrans = false
    result = MultiSet(application, op, field, regex, istrans)
    for _, res := range result {
        fmt.Println(res)
    }
    //read
    result = MultiGet(application)
    for _, res := range result {
        fmt.Println(res)
    }

    //del one
    op = false
    field = "baiduid"
    regex = " baiduid\\[([\\=A-Za-z0-9:]*?)\\] "
    istrans = true
    result = MultiSet(application, op, field, regex, istrans)
    for _, res := range result {
        fmt.Println(res)
    }
    //read
    result = MultiGet(application)
    for _, res := range result {
        fmt.Println(res)
    }

}








