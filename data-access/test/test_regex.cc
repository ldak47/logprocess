#include "gtest/gtest.h"
#include "gflags/gflags.h"
#include "glog/logging.h"
#include <string>
#include <regex>
#include "libconfig.h++"

std::string testnotice = "NOTICE: 17-06-22 16:46:47 errno[0] logId[3661423901] uri[/aese/index.php/api/search] user[127.0.0.1  ] refer[http://www.baidu.com]  cluster[nj03] tm_anti[0.02] baiduid[] cookieid[97F6589AC87B6D9EA78F81C92DEEE3DA] query[东营天气预报] ps_sid[] wise_sid[] sid[104493,100185,116336,104381,117189,116694,107317,117270,116810,117235,117119,117086,116996,117171,116992,117136,116309,115545,116689,116892,116387,115350,114276,115136,116412,116522,110085,117027,100458] net_type[1] mobile_browser[qq] mobile_browser_id[2] mobile_browser_version[6.3] srcid[4186] city_name[东营] province_name[山东] dspName[iphone] dsp[] tm_load_conf[0.22] crd[] tm_rewrite_group[0.03] So_ChangjingStrategy[0_pm | 0_mq | -1_mq] tm_stage_func[0.08 | 0.06 | 0.15] tm_rewrite_cardid[0.2] rw_cards[47033,47034,46019] tm_rewrite_srcida[0.21] make_query[0.09] make_query_fail[47034_17990_make_queryxiaoqi_make_fun] tm_gen_para[0.01] ral_req[So_GssSearch_东营天气预报_wisexmlnew_17990] tm_add_req_m[0.96] tm_add_req[1.11] null_res[So_GssSearch$$$gssSo_GssSearch东营天气预报wisexmlnew$$$47034_17990_ori_query:17990] exv[So_GssSearch$$$gssSo_GssSearch东营天气预报wisexmlnew$$$47034_17990_ori_query:17990] tm_get_res[18.1] ral_res[] tm_merge_res[0.01] tm_group_srcid[0.26] tm_get_card_res[19.23] tm_do_search[20] errmes[null cardid reuslt for main cardid 47034] resno[9] qid[12286720758457292737]";

std::string testnotice2 = "NOTICE: 17-07-16 12:40:58 errno[0] logId[3388079012] uri[/aese/index.php/api/search] user[127.0.0.1  ] refer[]  cluster[gzns] tm_anti[0.01] baiduid[41DD8EA59E667EF76591E893B95F5B3C:FG=1] cookieid[] query[小米手机专卖店地址] ps_sid[] wise_sid[110314_114550_117614_110136_116790_114743_109549_110770_107918_117619_117019_107320_117583_117331_117241_117457_117431_117416_116097_116309_115538_117774_116690_117553_116606_114819_116411_116523_110085_117027] sid[110314,114550,117614,110136,116790,114743,109549,110770,107918,117619,117019,107320,117583,117331,117241,117457,117431,117416,116097,116309,115538,117774,116690,117553,116606,114819,116411,116523,110085,117027] net_type[] mobile_browser[] mobile_browser_id[] mobile_browser_version[] srcid[4255] city_name[] province_name[] dspName[iphone] dsp[] tm_load_conf[0.16] tm_align_uri[0] tm_rewrite_group[0.02] So_ChangjingStrategy[0_pm | _mq | _mq | _mq | 0_am | _mq | _mq | _mq | 0_mq | 0_mq | 0_mq | 0_rank] tm_stage_func[0.01 | 0.01 | 0.01 | 0.01 | 0.02 | 0.02 | 0.01 | 0.01 | 0.22 | 0.08 | 0.11 | 0.01] tm_rewrite_cardid[0.09 | 0.06] rw_cards[4255,4282 | 4255,4282] tm_rewrite_srcida[0.1] make_query[0.04 | 0.02 | 0.02 | 0.05 | 0.04 | 0.04 | 0.15] tm_gen_para[0.01 | 0.01 | 0.01 | 0.01 | 0.01 | 0.01 | 0] ral_req[So_PoiSearch_小米手机专卖店地址_baidu_4255 | So_PoiSelectSearch_小米手机专卖店地址_baidu_4256 | So_PoiSubwaySearch_小米手机专卖店地址_baidu_4257 | So_PoiSearch_小米手机专卖店地址_baidu_4255 | So_PoiSelectSearch_小米手机专卖店地址_baidu_4256 | So_PoiSubwaySearch_小米手机专卖店地址_baidu_4257 | So_GssSearchArea_小米手机专卖店地址_tangram_8266] tm_add_req_m[0.11 | 0.07 | 0.07 | 0.02 | 0.01 | 0.01 | 0.48] tm_add_req[0.4 | 0.74] tm_get_res[317.56 | 0.09] ral_res[4282_4255_ori_query|4282_4256_ori_query|4282_4257_ori_query | 4255_4255_ori_query|4255_4256_ori_query|4255_4257_ori_query] tm_card_map4282[0.15] tm_merge_res[0.19 | 1.85] tm_group_srcid[0.11 | 0.69] tm_get_card_res[318.16 | 2.69] tm_get_main_res[318.28] tm_rewrite_srcidb[0.08] area_target_make_query[unconsumer_poi] make_query_fail[4255_8238_AreaTarget | 4255_8238_AreaTags] area_tags_make_query[unconsumer_poi] area_recom_err[miss_conf] area_info_make_query[area_make_query error!] make_query_err[area_info_make_query] kf_err[AreaInfo] use_ori_query[AreaInfo] tm_card_map4255[1.83] tm_get_son_res[3.4] tm_read_redis[0.01] is_headstyle_new[] is_show_big[] using_local[] Sort[] ClickNeed[] card_res[4282,4255] tm_sort[0.04] tm_proc_sort[0.01] tm_field_proc[0.05] card_ranked[default_group:4255] slv[uri:&UrlParaPack:] tm_sort_res[0.23] is_show_big_r[1] Sort_r[] ClickNeed_r[] tm_do_search[322.37] errmes[ok] qid[11750099392997691101]";

std::string testwarning = "'WARNING: 17-06-22 16:41:41 [/home/work/search/pae/php/phplib/sm/base/AELog.php:306] errno[0] logId[3630822974] uri[/aese/index.php/api/search] user[127.0.0.1  ] refer[http://www.baidu.com] cookie[]  cluster[nj02] stack[0-/home/work/search/pae/app/changjinghua/library/so/ChangjinghuaSearch.php:419 ; 1-/home/work/search/pae/app/changjinghua/library/so/ChangjinghuaSearch.php:1746 ; 2-/home/work/search/pae/app/changjinghua/library/so/ChangjinghuaSearch.php:1280 ; 3-/home/work/search/pae/app/changjinghua/models/service/page/Changjinghua.php:30 ; 4-/home/work/search/pae/php/phplib/saf/api/Service.php:163 ; 5-/home/work/search/pae/php/phplib/sm/api/changjinghua/Service.php:50 ; 6-/home/work/search/pae/php/phplib/saf/api/Server.php:116 ; 7-/home/work/search/pae/php/phplib/sns/flex/php/Wrapper.php:15 ; 8-/home/work/search/pae/php/phplib/sns/flex/Base.php:77 ; 9-/home/work/search/pae/php/phplib/sns/flex/Base.php:114 ; 10-/home/work/search/pae/app/aese/library/so/ChangjinghuaSearchEnt.php:245 ; 11-/home/work/search/pae/app/aese/library/so/DefaultSearch.php:241 ; 12-/home/work/search/pae/app/aese/models/service/page/SearchStart.php:214 ; 13-/home/work/search/pae/app/aese/actions/api/Search.php:18 ; 14-/home/work/search/pae/php/phplib/sm/base/PageAction.php:35 ; 15-: ; 16-: ; 17-: ; 18-/home/work/search/pae/webroot/aese/index.php:12] msg[loss key 6896 ] qid[11585745846684178107]'";

TEST(test_regex, test_phone) {
    std::regex reg2("(\\d{1,3}):(\\d{1,3}):(\\d{1,3}):(\\d{1,3})");
    std::string ip = "0:11:222:333";
    std::smatch m;
    regex_search(ip, m, reg2);
    LOG(INFO) << m.str();
    LOG(INFO) << m.str(1);
    LOG(INFO) << m.str(2);
    LOG(INFO) << m.str(3);
    LOG(INFO) << m.str(4);
}

TEST(test_regex, test_chi) {
    std::string text = "vice jax 隔壁的 teemo, 老王  ge bi";
    std::regex reg(" ([\u4e00-\u9fa5]+) ");
    std::sregex_iterator pos(text.cbegin(), text.cend(), reg);
    std::sregex_iterator end;
    for (; pos != end; ++pos) {
        LOG(INFO) << "match: " << pos->str();
        LOG(INFO) << "tag: " << pos->str(1);
    }
}

TEST(test_regex, test_notice) {
    libconfig::Config cfg;
    try {
        cfg.readFile("../conf/data-access.cfg");
    } catch (...) {
        LOG(WARNING) << "parse fail";
        return;
    }
    const libconfig::Setting &root = cfg.getRoot();
        
    std::regex reg(" srcid\\[(\\d+)\\] ");
    std::smatch res;
    regex_search(testnotice, res, reg);
    LOG(INFO) << "srcid: " << res.str(1);

    reg = (" sid\\[([0-9,]*)\\] ");
    regex_search(testnotice, res, reg);
    LOG(INFO) << "sid: " << res.str(1);

    reg = (" city_name\\[([\u4e00-\u9fa5]+)\\] ");
    regex_search(testnotice, res, reg);
    LOG(INFO) << "city_name: " << res.str(1);

    reg = (" province_name\\[([\u4e00-\u9fa5]+)\\] ");
    regex_search(testnotice, res, reg);
    LOG(INFO) << "province_name: " << res.str(1);

    const libconfig::Setting &q_cfg =  root["preprocess_action"]["weather"][4];
    std::string q_r;
    q_cfg.lookupValue("regex", q_r);
    LOG(INFO) << q_r;
    reg = (q_r.c_str());
    regex_search(testnotice2, res, reg);
    LOG(INFO) << "query: " << res.str(1);

    reg = (" qid\\[(\.+?)\\]$");
    regex_search(testnotice2, res, reg);
    LOG(INFO) << "qid: " << res.str(1);

    const libconfig::Setting &cookie_cfg =  root["preprocess_action"]["weather"][9];
    std::string cookie_r;
    cookie_cfg.lookupValue("regex", cookie_r);
    //reg = (" cookieid\\[(\.*?)\\] ");
    reg = cookie_r.c_str();
    regex_search(testnotice, res, reg);
    LOG(INFO) << "cookieid: " << res.str(1);

    const libconfig::Setting &baiduid_cfg =  root["preprocess_action"]["weather"][8];
    std::string baiduuid_r;
    baiduid_cfg.lookupValue("regex", baiduuid_r);
    //reg = (" baiduid\\[(\.*?)\\] ");
    reg = (baiduuid_r.c_str());
    regex_search(testnotice2, res, reg);
    LOG(INFO) << "baiduid: " << res.str(1);

    const libconfig::Setting &cluster_cfg =  root["preprocess_action"]["weather"][6];
    std::string cluster_r;
    cluster_cfg.lookupValue("regex", cluster_r);
    LOG(INFO) << cluster_r;
    //reg = (" baiduid\\[(\.*?)\\] ");
    reg = (cluster_r.c_str());
    regex_search(testnotice2, res, reg);
    LOG(INFO) << "cluster: " << res.str(1);

    const libconfig::Setting &time_cfg =  root["preprocess_action"]["weather"][10];
    std::string time_r;
    time_cfg.lookupValue("regex", time_r);
    LOG(INFO) << time_r;
    //reg = (" baiduid\\[(\.*?)\\] ");
    reg = (time_r.c_str());
    regex_search(testnotice, res, reg);
    LOG(INFO) << "time: " << res.str(1);

    const libconfig::Setting &resno_cfg =  root["preprocess_action"]["weather"][7];
    std::string resno_r;
    resno_cfg.lookupValue("regex", resno_r);
    LOG(INFO) << resno_r;
    //reg = (" baiduid\\[(\.*?)\\] ");
    reg = (resno_r.c_str());
    regex_search(testnotice2, res, reg);
    std::string resno = res.str(1);
    LOG(INFO) << "resno: " << res.str(1) << ", " << resno.length();

}

TEST(test_regex, test_warning) {
    std::regex reg("\\w+");
    std::string r1 = "北京天气";
    std::string r21 = "1北京天气预报1";
    std::string r22 = "北京天气预报";

    std::smatch res;
    reg = ("^北京天气\.+");
    LOG(INFO) << regex_search(r21, res, reg);
    LOG(INFO) << regex_search(r22, res, reg);

    std::string r3 = "天气预报";
    reg = ("\.+天气预报$");
    LOG(INFO) << regex_search(r21, res, reg);
    LOG(INFO) << regex_search(r22, res, reg);
}

int main (int argc, char *argv[]) {
    google::SetStderrLogging(google::GLOG_FATAL);
    FLAGS_logbufsecs = 0;
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
