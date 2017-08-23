package transmit

import (
    "fmt"
    "log"
    "time"
    "net"
    "net/http"
    "io/ioutil"
    "bytes"
)

type RemoteResult struct {
    Addr string
    Result string
}

func Transmit (postbody *bytes.Buffer, url string, addr string, resultChan chan RemoteResult) {
    client := &http.Client{
        Transport: &http.Transport{
            Dial: func(netw, addr string) (net.Conn, error) {
                conn, err := net.DialTimeout(netw, addr, time.Second * 20)
                if err != nil {
                    return nil, err
                }
                conn.SetDeadline(time.Now().Add(time.Second * 20))
                return conn, nil
            },
            ResponseHeaderTimeout: time.Second * 20,
        },
    }
    request,err := http.NewRequest("POST", url, postbody)
    request.Header.Set("Content-Type", "application/json, text/html")
    if err != nil {
        var errmsg = "client construct fail: "
        log.Println(errmsg, err)
        fmt.Println(errmsg, err)
        resultChan <- RemoteResult{Addr: addr, Result: errmsg}
        return
    }

    response,err := client.Do(request)
    if err != nil {
        var errmsg = "client request fail: "
        log.Println(errmsg, err)
        fmt.Println(errmsg, err)
        resultChan <- RemoteResult{Addr: addr, Result: errmsg}
        return
    }

    defer response.Body.Close()
    resbody,err := ioutil.ReadAll(response.Body)
    if err != nil {
        var errmsg = "client read fail: "
        log.Println(errmsg, err)
        fmt.Println(errmsg, err)
        resultChan <- RemoteResult{Addr: addr, Result: errmsg}
        return
    }
    resultChan <- RemoteResult{Addr: addr, Result: string(resbody)}
}

