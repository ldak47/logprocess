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

type SourceFrom int
const (
    Rocksdb SourceFrom = 0;
    QueryTrie SourceFrom = 1;
    Redis SourceFrom = 2;
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
    Description string 
}

type GetRequest struct {
    Application string
    Cond CheckListCond
    Selectfields []string
    Sortfield string
    Getfrom SourceFrom
    Startime uint64
    Endtime uint64
    Start uint32
    End uint32
}

func GetLogProcessStat (addr string, application string, srcids []SrcidConfig, sewords []ScanCondSeWord, lbs []LocationInfo, cookies []CookieConfig, clusters []ClusterConfig, sids []SidConfig, resnos []ResnoConfig, startime uint64, endtime uint64, startindex uint32, endindex uint32, selectfields []string, sortfield string, resultChan chan transmit.RemoteResult) {
    var module string = "process.ProcessService."
    var method string = "GetLogProcessStat"
    var url string = strings.Join([]string{"http://", addr, "/", module, method}, "")

    var postarg GetRequest
    postarg.Application = application
    postarg.Startime = startime
    postarg.Endtime = endtime
    postarg.Start = startindex
    postarg.End = endindex
    postarg.Getfrom = Rocksdb
    postarg.Selectfields = selectfields
    postarg.Sortfield = sortfield
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
    transmit.Transmit(postbody, url, addr, resultChan)
}

func MultiGet (application string, srcids []SrcidConfig, sewords []ScanCondSeWord, lbs []LocationInfo, cookies []CookieConfig, clusters []ClusterConfig, sids []SidConfig, resnos []ResnoConfig, startime uint64, endtime uint64, startindex uint32, endindex uint32, selectfields []string, sortfield string) ([]transmit.RemoteResult) {
    addrs := GetProcessers()
    resultChan := make(chan transmit.RemoteResult, len(addrs))
    defer close(resultChan)
    var result []transmit.RemoteResult

    for _, addr := range addrs {
        go GetLogProcessStat(addr, application, srcids, sewords, lbs, cookies, clusters, sids, resnos, startime, endtime, startindex, endindex, selectfields, sortfield, resultChan)
    }

    for i := 0; i < len(addrs); i++ {
        res := <-resultChan
        result = append(result, res)
    }

    return result
}

func main () {
    var application string = "weather"
    var srcids []SrcidConfig
    var sewords []ScanCondSeWord
    var lbs []LocationInfo
    var cookies []CookieConfig
    var clusters []ClusterConfig
    var sids []SidConfig
    var resnos []ResnoConfig

    var srcidadd SrcidConfig
    srcidadd.Srcid = "4494"
    srcidadd.Cond = condEqual
    srcids = append(srcids, srcidadd)

    var resno ResnoConfig
    resno.Resno = ""
    resno.Cond = condEqual
    resnos = append(resnos, resno)

    var startime uint64 = 1502282880
    var endtime uint64 = 1503045060
    var startindex uint32 = 0
    var endindex uint32 = 1000

    var selectfields []string
    selectfields = append(selectfields, "srcid")
    selectfields = append(selectfields, "query")
    selectfields = append(selectfields, "timestamp")
    selectfields = append(selectfields, "resno")

    var sortfield string = "timestamp"

    result := MultiGet(application, srcids, sewords, lbs, cookies, clusters, sids, resnos, startime, endtime, startindex, endindex, selectfields, sortfield)
    for _, res := range result {
        fmt.Println(res)
    }
}
