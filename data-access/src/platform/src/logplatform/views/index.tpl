


<div id = "content">
<div id="content-header" style="text-align:center">
    <h1>日志监控平台</h1>
</div>

<div id="content-container">
    <div>
        <h4 class="heading-inline" style="text-align:center">日志监控
            &nbsp;&nbsp;<small>{{.time}}</small>
            &nbsp;&nbsp;
        </h4>
    </div>
    <br />

    <div id="nav_warp">
        <div id="nav">
            <ul style="margin:10; text-align:center">
                <li class="nav_line" style="isplay:inline;float:left;width:130px">
                    <a href="/pv">查看当前pv</a>
                </li>
                <li class="nav_line" style="display:inline;float:left;width:130px">
                    <a href="/checklist">报警list</a>
                </li>
                <li class="nav_line" style="display:inline;float:left;width:130px">
                    <a href="/select">复合查询</a>
                </li>
                <li class="nav_line" style="display:inline;float:left;width:130px">
                    <a href="/storage">数据存储设置</a>
                </li>
                <li class="nav_line" style="display:inline;float:left;width:130px">
                    <a href="/application">增加类目及配置</a>
                </li>
                <li class="nav_line" style="display:inline;float:left;width:130px">
                    <a href="/other">其他设置</a>
                </li>
            </ul>
        </div>


        <div class="col-md-2 col-sm-6">
            <a href="/stargaze/roadmonit/totalmileage" class="dashboard-stat primary">
                <div class="visual">
                    <i class="fa fa-star"></i>
                </div> <!-- /.visual -->

                <div class="details">
                    <span class="content">查看当前pv</span>
                </div> <!-- /.details -->
            </a> <!-- /.dashboard-stat -->
        </div> <!-- /.col-md-3 -->

        <div class="col-md-2 col-sm-6">
            <a href="/stargaze/roadmonit/totalmileage" class="dashboard-stat primary">
                <div class="visual">
                    <i class="fa fa-star"></i>
                </div> <!-- /.visual -->
                <div class="details">
                    <span class="content">快递</span>
                    <span class="value">100000000</span>
                </div> <!-- /.details -->
            </a> <!-- /.dashboard-stat -->
        </div> <!-- /.col-md-3 -->

        <div class="col-md-2 col-sm-6">
            <a href="/stargaze/roadmonit/totalmileage" class="dashboard-stat secondary">
                <div class="visual">
                    <i class="fa fa-clock-o"></i>
                </div> <!-- /.visual -->
                <div class="details">
                    <span class="content">天气当前pv</span>
                    <span class="value">10000000</span>
                </div> <!-- /.details -->

                <i class="fa fa-play-circle more hide">主要：4178</i>

            </a> <!-- /.dashboard-stat -->

        </div> <!-- /.col-md-3 -->

        <div class="col-md-2 col-sm-6">

            <a href="/stargaze/roadmonit/totalmileage" class="dashboard-stat tertiary">
                <div class="visual">
                    <i class="fa fa-clock-o"></i>
                </div> <!-- /.visual -->

                <div class="details">
                    <span class="content">快递当前pv</span>
                    <span class="value">10000000</span>
                </div> <!-- /.details -->

                <i class="fa fa-play-circle more hide">主要：4155</i>

            </a> <!-- /.dashboard-stat -->

        </div> <!-- /.col-md-3 -->

        <div class="col-md-4 col-sm-6">

            <a href="/stargaze/roadmonit/citymileage" class="dashboard-stat">
                <div class="visual">
                    <i class="fa fa-money"></i>
                </div> <!-- /.visual -->

                <div class="details">
                    <span class="content">Diff</span>
                    <span class="value">0</span>
                </div> <!-- /.details -->

                <i class="fa fa-play-circle more">主要：
                </i>
            </a> <!-- /.dashboard-stat -->
        </div> <!-- /.col-md-3 -->
    </div> <!-- /.row -->

    <div class="row">
        <div class="portlet">
            <!--div class="portlet-header">
                <h3>
                    <i class="fa fa-bar-chart-o"></i>
                    全国路网监控
                </h3>
            </div--> <!-- /.portlet-header -->

            <div class="portlet-content">
                <div id="map-search-bar" style="display:none;">
                    <form id="search-form" action="/stargaze/" method="get">
                        <input type="hidden" name="year" id="year" value="2017"/>
                        <input type="hidden" name="month" id="month" value="7"/>
                        <input type="hidden" name="city" id="city" value="26"/>
                        <div class="pull-left">
                            <div class="form-group">
                                <!-- 年份：
                                <div class="btn-group ">
                                    <button class="btn btn-default btn-sm dropdown-toggle" type="button" data-toggle="dropdown">
                                        <i class="fa fa-clock-o"></i>  &nbsp;
                                {%$param['year']%} <span class="caret"></span>
                            </button>
                            <ul id="year-list" class="dropdown-menu" role="menu">
                                <li><a href="javascript:;">2015</a></li>
                                <li><a href="javascript:;">2016</a></li>
                            </ul>
                        </div>
                        &nbsp;&nbsp;
                        月份：
                        <div class="btn-group ">
                            <button class="btn btn-default btn-sm dropdown-toggle" type="button" data-toggle="dropdown">
                                <i class="fa fa-clock-o"></i>  &nbsp;
                                {%$param['month']%} <span class="caret"></span>
                            </button>
                            <ul id="month-list" class="dropdown-menu" role="menu">
                                <li><a href="javascript:;">1</a></li>
                                <li><a href="javascript:;">2</a></li>
                                <li><a href="javascript:;">3</a></li>
                                <li><a href="javascript:;">4</a></li>
                                <li><a href="javascript:;">5</a></li>
                                <li><a href="javascript:;">6</a></li>
                                <li><a href="javascript:;">7</a></li>
                                <li><a href="javascript:;">8</a></li>
                                <li><a href="javascript:;">9</a></li>
                                <li><a href="javascript:;">10</a></li>
                                <li><a href="javascript:;">11</a></li>
                                <li><a href="javascript:;">12</a></li>
                            </ul>
                        </div>&nbsp;&nbsp;&nbsp;&nbsp; -->
                                路网：
                                <label class="checkbox-inline" style="color:#519ECD;">
                                    <input type="checkbox" name="bd" value="1"}>百度</label>
                                <label class="checkbox-inline" style="color:#EE9C9B;">
                                    <input type="checkbox" name="gd" value="1">高德</label>
                                <label class="checkbox-inline" style="color:#EE9C9B;">
                                    <input type="checkbox" name="gddiff" value="1">差分</label>

                                <!--label class="checkbox-inline">
                                    <input type="checkbox" id="inlineCheckbox3" value="option3">在建路网</label>
                                <label class="checkbox-inline">
                                    <input type="checkbox" id="inlineCheckbox3" value="option3">产品数据</label-->
                                &nbsp;&nbsp;&nbsp;&nbsp;等级：
                                <label class="checkbox-inline">
                                    <input type="checkbox" name="g01" value="1"}>高速</label>
                                <label class="checkbox-inline">
                                    <input type="checkbox" name="g02" value="1">国道</label>
                                <label class="checkbox-inline">
                                    <input type="checkbox" name="g03" value="1"}>省道</label>
                                <label class="checkbox-inline">
                                    <input type="checkbox" name="g04" value="1">县乡村道</label>
                                <label class="checkbox-inline">
                                    <input type="checkbox" name="g05" value="1">非导航道路</label>
                            </div>
                        </div>
                    </form>

                    <div class="pull-right">
                        <div class="input-group">
                            <label class="checkbox-inline" id="zoom" style="margin-right:5px;">缩放等级：10</label>
                            <button class="btn btn-warning btn-sm btn-search" id="btn-map-fullscreen" type="button"><i class="glyphicon glyphicon-fullscreen"></i></button>&nbsp;
                            <input type="text" placeholder="请输入查询信息..." class="search-input  input-medium" value="" id="search-key">
                            <div class="btn-group">
                                <button class="btn btn-primary btn-sm btn-search " id="btn-map-search" type="button"><i class="fa fa-search"></i></button>
                            </div>
                        </div>
                    </div>
                    <div class="clear"></div>
                </div>
                <div id="area-chart" class="chart-holder" style="display:none;"></div>

                <div class="clear"></div>
                <div id="map" class="chart-holder" style="display:none;"></div> <!-- /#bar-chart -->
            </div> <!-- /.portlet-content -->
        </div> <!-- /.portlet -->
    </div> <!-- /.row -->

</div> <!-- /#content-container -->
</div>
<script>
    //var paramCity = '{%$param['city']%}';
    //var markPoint = JSON.parse('{%$markPoint%}');
    console.log("columbs");
</script>


