package main

import (
    "fmt"
    "log"
    "encoding/json"
    "bytes"
    "strings"
    "./transmit"
    "strconv"
)

type CondType int
const (
    condIn CondType = 1;
    condNotIn CondType = 2;
    condIs CondType = 3;
    condExist CondType = 4;
    condNotExist CondType = 5;
    condEqual CondType = 6;
    condPartEqual CondType = 7;
    condNotEqual CondType = 8;
    condLarger CondType = 9;
    condSmaller CondType = 10;
    condIsFront CondType = 11;
    condIsBehind CondType = 12;
)

type TypeConfig struct {
    Type string
    Cond CondType
}

type SrcidConfig struct {
    Srcid string
    Cond CondType
}

type ScanCondSeWord struct {
    Searchword string
    Cond CondType
}

type LocationInfo struct {
    Province string
    City string 
    Cond CondType
}

type CookieConfig struct {
    Cookie string
    Cond CondType
}

type ClusterConfig struct {
    Cluster string
    Cond CondType
}

type SidConfig struct {
    Sid string
    Cond CondType
}

type ResnoConfig struct {
    Resno string
    Cond CondType
}

type CheckListCond struct {
    Srcids []SrcidConfig `json: ",omitempty"`
    Sewords []ScanCondSeWord `json: ",omitempty"`
    Lbs []LocationInfo `json: ",omitempty"`
    Cookie []CookieConfig `json: ",omitempty"`
    Cluster []ClusterConfig `json: ",omitempty"`
    Sid []SidConfig `json: ",omitempty"`
    Resno []ResnoConfig `json: ",omitempty"`
    Type []TypeConfig `json: "omitempty"`
    Description string 
}

type GetRequest struct {
    Application string
    Index uint32  `json: ",omitempty"`
}

type SetRequest struct {
    Op bool
    Index uint32 `json: ",omitempty"`
    Application string
    Cond CheckListCond
}

func GetProcessers () ([]string) {
    var addrs []string

    for i := 0; i < 40; i++ {
        var idx string = strconv.Itoa(i + 8500)
        var addr string = "10.99.196.105:" + idx
        addrs = append(addrs, addr)
    }

    for i := 0; i < 120; i++ {
        var idx string = strconv.Itoa(i + 8500)
        var addr string = "10.120.208.11:" + idx
        addrs = append(addrs, addr)
    }

    return addrs
}
func ChecklistSet (op bool, index uint32, addr string, application string, srcids []SrcidConfig, sewords []ScanCondSeWord, lbs []LocationInfo, cookies []CookieConfig, clusters []ClusterConfig, sids []SidConfig, resnos []ResnoConfig, types []TypeConfig, descript string, resultChan chan transmit.RemoteResult) {
    var module string = "process.ProcessService."
    var method string = "SetCheckListConfig"
    var url string = strings.Join([]string{"http://", addr, "/", module, method}, "")

    var postarg SetRequest

    postarg.Application = application
    postarg.Op = op
    postarg.Index = index
    var cond CheckListCond
    if len(srcids) > 0 {
        cond.Srcids = srcids
    } else {
        cond.Srcids = []SrcidConfig{}
    }
    if len(sewords) > 0 {
        cond.Sewords = sewords
    } else {
        cond.Sewords = []ScanCondSeWord{}
    }
    if len(lbs) > 0 {
        cond.Lbs = lbs
    } else {
        cond.Lbs = []LocationInfo{}
    }
    if len(cookies) > 0 {
        cond.Cookie = cookies
    } else {
        cond.Cookie = []CookieConfig{}
    }
    if len(clusters) > 0 {
        cond.Cluster = clusters
    } else {
        cond.Cluster = []ClusterConfig{}
    }
    if len(sids) > 0 {
        cond.Sid = sids
    } else {
        cond.Sid = []SidConfig{}
    }
    if len(resnos) > 0 { 
        cond.Resno = resnos
    } else {
        cond.Resno = []ResnoConfig{}
    }
    if len(types) > 0 {
        cond.Type = types
    } else {
        cond.Type = []TypeConfig{}
    }

    cond.Description = descript
    postarg.Cond = cond

    jsonarg, err := json.Marshal(postarg)
    if err != nil {
        var errmsg = "json Marshal fail: "
        log.Println(errmsg, err)
        fmt.Println(errmsg, err)
        resultChan <- transmit.RemoteResult{Addr: addr, Result: errmsg}
        return
    }
    postbody := bytes.NewBuffer([]byte(jsonarg))
fmt.Println(postbody)
    transmit.Transmit(postbody, url, addr, resultChan)
}

func ChecklistGet (addr string, application string, index uint32, resultChan chan transmit.RemoteResult) {
    var module string = "process.ProcessService."
    var method string = "GetCheckListConfig"
    var url string = strings.Join([]string{"http://", addr, "/", module, method}, "")

    var postarg GetRequest
    postarg.Application = application
    postarg.Index = index

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

func MultiGet (application string, index uint32) ([]transmit.RemoteResult) {
    addrs := GetProcessers()
    resultChan := make(chan transmit.RemoteResult, len(addrs))
    defer close(resultChan)
    var result []transmit.RemoteResult

    for _, addr := range addrs {
        go ChecklistGet(addr, application, index, resultChan)
    }

    for i := 0; i < len(addrs); i++ {
        res := <-resultChan
        result = append(result, res)
    }

    return result
}

func MultiSet (op bool, index uint32, application string, srcids []SrcidConfig, sewords []ScanCondSeWord, lbs []LocationInfo, cookies []CookieConfig, clusters []ClusterConfig, sids []SidConfig, resnos []ResnoConfig, types []TypeConfig, descript string) ([]transmit.RemoteResult) {
    addrs := GetProcessers()
    resultChan := make(chan transmit.RemoteResult, len(addrs))
    defer close(resultChan)
    var result []transmit.RemoteResult

    for _, addr := range addrs {
        go ChecklistSet(op, index, addr, application, srcids, sewords, lbs, cookies, clusters, sids, resnos, types, descript, resultChan)
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
    var index uint32 = 0    
    result := MultiGet(application, index)
    for _, res := range result {
        fmt.Println(res)
    }

    //add one
    var descript string = "new weather"
    var srcids []SrcidConfig
    var sewords []ScanCondSeWord
    var lbs []LocationInfo
    var cookies []CookieConfig
    var clusters []ClusterConfig
    var sids []SidConfig
    var resnos []ResnoConfig
    var types []TypeConfig

    var srcidone SrcidConfig
    srcidone.Srcid = "4514"
    srcidone.Cond = condEqual
    srcids = append(srcids, srcidone)

    var typeone TypeConfig
    typeone.Type = "new"
    typeone.Cond = condEqual
    types = append(types, typeone)

    var resnone ResnoConfig
    resnone.Resno = ""
    resnone.Cond = condNotEqual
    resnos = append(resnos, resnone)

    result = MultiSet(true, index, application, srcids, sewords, lbs, cookies, clusters, sids, resnos, types, descript)
    for _, res := range result {
        fmt.Println(res)
    }
    //read
    result = MultiGet(application, index)
fmt.Println("add one: ")
    for _, res := range result {
        fmt.Println(res)
    }
    return
}
