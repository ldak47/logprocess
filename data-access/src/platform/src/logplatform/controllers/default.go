package controllers

import (
    "fmt"
    "time"
	"github.com/astaxie/beego"
    "github.com/astaxie/beego/config"
)

type MainController struct {
	beego.Controller
}

func (c *MainController) Get() {
    cfg, err := config.NewConfig("ini", "conf/app.conf")
    if err != nil {
        fmt.Println("config.NewConfig fail: %q", err)
    }
    fmt.Println("dataaccess_addr: ", cfg.DefaultString("dataaccess_addr", "127.0.0.1"))

    c.Data["time"] = (time.Now().Format("2006-01-02 03:04:05 PM"))
	c.TplName = "index.tpl"
}

