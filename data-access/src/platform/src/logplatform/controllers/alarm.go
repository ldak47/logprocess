package controllers

import (
    "fmt"
    //"reflect"
    "crypto/tls"
    "github.com/go-gomail/gomail"
    "log"
    "bytes"
    "strings"
    "encoding/json"
    "github.com/astaxie/beego"
    "transmit"
)

type AlarmController struct {
    beego.Controller
}

type SourceType int
const (
    Rocksdb SourceType = 0;
    QueryTrie SourceType = 1;
    Redis SourceType = 2;
)

type Res_Status int
const (
    SUCCEED Res_Status = 0;
    FAILED Res_Status = 1;
)

type GetRequest struct {
    Getfrom SourceType
    Querystr string
    Srcid string
    Startime uint64
    Endtime uint64
}

type RemoteResult struct {
    Addr string
    Result string
}

type MapEntry struct {
    Key string
    Value string
}

type MapEntries struct {
    Item []MapEntry
}

type QueryTrieResult struct {
    Status Res_Status
    Count uint64
    Iteminfos []MapEntries
}

func JudgeFromQueryTrie (addr string, query string, srcid string, resultChan chan transmit.RemoteResult) {
    var module string = "process.ProcessService."
    var method string = "GetLogProcessStat"
    var url string = strings.Join([]string{"http://", addr, "/", module, method}, "")
    var postarg GetRequest
    postarg.Querystr = query
    postarg.Srcid = srcid
    postarg.Getfrom = QueryTrie
    postarg.Startime = 1501841400
    postarg.Endtime = 1501841400
    jsonarg, err := json.Marshal(postarg)
    if err != nil {
        var errmsg = "json Marshal fail: "
        log.Println(errmsg, err)
        fmt.Println(errmsg, err)
        resultChan <- transmit.RemoteResult{Addr: addr, Result: errmsg}
        return
    } else {
        postbody := bytes.NewBuffer([]byte(jsonarg))
        transmit.Transmit(postbody, url, addr, resultChan)
    }
}

func SendToMail (body string) error {
    var user string = "ldak47@126.com"
    var passwd string = "ldak47"
    m := gomail.NewMessage()
    m.SetAddressHeader("From", user, "log-platform")
    m.SetHeader("To", m.FormatAddress(user, "liudong"))
    m.SetHeader("Subject", "log-platform-alarm for new weather")
    m.SetBody("text/html", body)

    d := gomail.NewPlainDialer("smtp.126.com", 25, user, passwd)
    d.TLSConfig = &tls.Config{InsecureSkipVerify: true}
    err := d.DialAndSend(m) 
    if err != nil {
        fmt.Println(err)
    }

    return err
}

func JudgeQueryTrie (query string, srcid string) bool {
    addrs := strings.FieldsFunc(*dataprocess_addrs, split)
    resultChan := make(chan transmit.RemoteResult, len(addrs))
    defer close(resultChan)
    //var result []transmit.RemoteResult
    for _, addr := range addrs {
        go JudgeFromQueryTrie(addr, query, srcid, resultChan)
    }
    var cnt uint64 = 0
    for i := 0; i < len(addrs); i++ {
        res := <-resultChan
        var temp QueryTrieResult
        json.Unmarshal([]byte(res.Result), &temp)
        cnt += temp.Count
        //result = append(result, res)
    }
    if (cnt > 0) {
        var body string = `
                <html>
                <body>
                <h3>
                ` + query + "|" + 
                srcid + `
                "search has not be reretrieval!"
                </h3>
                </body>
                </html>
                `
        SendToMail(body)
        return true
    } else {
        return false
    }
}

func (c *AlarmController) Post () {
    conf.Parse()
    var m map[string]string
    json.Unmarshal(c.Ctx.Input.RequestBody, &m)
    var query string = m["query"]
    var srcid string = m["srcid"]

    var res bool = JudgeQueryTrie(query, srcid)
    c.Data["json"] = map[string]interface{}{"success":0,"result":res}
    c.ServeJSON()
}
