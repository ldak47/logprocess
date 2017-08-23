package controllers

import (
    "fmt"
    "os"
    "encoding/json"
    "github.com/astaxie/beego"
)

type KuozhaoController struct {
    beego.Controller
}

func appendToFile(query string) error {
    query = query + "\n"
    f, err := os.OpenFile("/home/work/kuozhao.txt", os.O_WRONLY, 0644)
    if err != nil {
        fmt.Println("cacheFileList.yml file create failed. err: " + err.Error())
    } else {
        n, _ := f.Seek(0, os.SEEK_END)
        _, err = f.WriteAt([]byte(query), n)
    }   
    defer f.Close()
    return err
}

func (c *KuozhaoController) Post () {
    var m map[string]string
    json.Unmarshal(c.Ctx.Input.RequestBody, &m)
    var query string = m["query"]
    appendToFile(query)

    c.ServeJSON()
}
