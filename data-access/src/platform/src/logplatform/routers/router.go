package routers

import (
	"logplatform/controllers"
	"github.com/astaxie/beego"
)

func init() {
    beego.Router("/", &controllers.MainController{})
    beego.Router("/health", &controllers.HealthController{})
    beego.Router("/relax", &controllers.RelaxController{})
    beego.Router("/alarm", &controllers.AlarmController{})
    beego.Router("/kuozhao", &controllers.KuozhaoController{})
}
