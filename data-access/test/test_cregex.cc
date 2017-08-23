#include <regex.h>
#include "gtest/gtest.h"
#include "client_hook.h"
#include "timer.h"

std::string testnotice = "NOTICE: 17-06-22 16:46:47 errno[0] logId[3661423901] uri[/aese/index.php/api/search] user[127.0.0.1  ] refer[http://www.baidu.com]  cluster[nj03] tm_anti[0.02] baiduid[ADFASDFadsfsaf234324324:453:A] cookieid[97F6589AC87B6D9EA78F81C92DEEE3DA] query[东营天气预报] ps_sid[] wise_sid[] sid[104493,100185,116336,104381,117189,116694,107317,117270,116810,117235,117119,117086,116996,117171,116992,117136,116309,115545,116689,116892,116387,115350,114276,115136,116412,116522,110085,117027,100458] net_type[1] mobile_browser[qq] mobile_browser_id[2] mobile_browser_version[6.3] srcid[4186] city_name[东营] province_name[山东] dspName[iphone] dsp[] tm_load_conf[0.22] crd[] tm_rewrite_group[0.03] So_ChangjingStrategy[0_pm | 0_mq | -1_mq] tm_stage_func[0.08 | 0.06 | 0.15] tm_rewrite_cardid[0.2] rw_cards[47033,47034,46019] tm_rewrite_srcida[0.21] make_query[0.09] make_query_fail[47034_17990_make_queryxiaoqi_make_fun] tm_gen_para[0.01] ral_req[So_GssSearch_东营天气预报_wisexmlnew_17990] tm_add_req_m[0.96] tm_add_req[1.11] null_res[So_GssSearch$$$gssSo_GssSearch东营天气预报wisexmlnew$$$47034_17990_ori_query:17990] exv[So_GssSearch$$$gssSo_GssSearch东营天气预报wisexmlnew$$$47034_17990_ori_query:17990] tm_get_res[18.1] ral_res[] tm_merge_res[0.01] tm_group_srcid[0.26] tm_get_card_res[19.23] tm_do_search[20] errmes[null cardid reuslt for main cardid 47034] resno[9] qid[12286720758457292737]";

std::string t = "NOTICE: 17-07-30 18:55:22 errno[0] logId[4187985580] uri[/aese/index.php/api/search] user[127.0.0.1  ] refer[http://www.baidu.com]  cluster[st01] tm_anti[0.02] baiduid[] cookieid[5912111DB3F38896AEC851F3EA745EAA] query[天气预报] ps_sid[] wise_sid[] sid[118135,102571,114551,117616,117793,106198,100100,118155,107316,118144,117852,117348,117579,117331,117238,117456,117434,118133,118054,115534,118103,117555,114893,117635,117474,116523,110085,117027] net_type[1] mobile_browser[applewebkit] mobile_browser_id[3] mobile_browser_version[600] srcid[4494] city_name[黄冈] province_name[湖北] dspName[iphone] dsp[] tm_load_conf[0.27] crd[] tm_rewrite_group[0.06] So_ChangjingStrategy[0_pm | 0_mq] tm_stage_func[0.02 | 0.14] tm_rewrite_cardid[0.18] rw_cards[33766,4494] tm_rewrite_srcida[0.2] make_query[0.18] tm_gen_para[0.01] ral_req[So_GssSearch_天气预报_tangram_33766] tm_add_req_m[0.41] tm_add_req[0.6] null_res[So_GssSearch$$$gssSo_GssSearch天气预报tangram$$$33766_33766_ori_query:33766] exv[So_GssSearch$$$gssSo_GssSearch天气预报tangram$$$33766_33766_ori_query:33766] tm_get_res[9.94] ral_res[] tm_merge_res[0.02] tm_group_srcid[0.2] tm_get_card_res[10.57] tm_do_search[11.39] errmes[null cardid reuslt for main cardid 33766] resno[9] qid[11556439480312291735]";

std::string s = "'NOTICE: 17-07-30 22:34:45 errno[0] logId[3356790768] uri[/weather/index.php/api/search?srcid=4514&query=%E7%A7%A6%E7%9A%87%E5%B2%9B%E5%A4%A9%E6%B0%94%E9%A2%84%E6%8A%A515%E5%A4%A9] user[10.212.139.17  ] refer[http://www.baidu.com] cookie[]  optime[1501425285.570] client_ip[10.212.139.17] local_ip[10.212.214.41] product[ORP] subsys[ORP] module[weather] uniqid[0] cgid[0] uid[0] cluster[hz01] query[秦皇岛天气预报15天] srcid[4514] city_name[石家庄] province_name[河北] qid[11175537011991348896] qid64x[9b177bb3adf986a0] sid[] caller[default] tn[baidu] dsp[iphone] qu_hit[1] select_day[2017-07-30] errno[0] searchstarttime[1501425285571.6] searchendtime[1501425285696.1] tm_cost[124.49] from[action_api] searchexetime[1501425285571] searchcalltime[1501425285571] searchrettime[1501425285696] phpcost[126] starttime[1501425285570] endtime[1501425285696]'";

TEST (test, direct) {
    common::TimeCalcer tc;
    std::string field = "srcid[";
    size_t idx = testnotice.find(field);
    std::string res = "";
    if (idx != std::string::npos) {
        for (size_t i = idx + field.length(); i < testnotice.length(); i++) {
            if (testnotice[i] == ']') {
                break;
            } else {
                res += testnotice[i];
            }
        }
    }
    double tm = tc.Calc();
    LOG(INFO) << std::fixed << res << " cost " << tm;
}

TEST (test, cregex) {
    regmatch_t pmatch[1];
    const size_t nmatch = 1;
    regex_t reg;
    const char *pattern = "([0-9]+\-[0-9]+\-[0-9]+ [0-9]+\:[0-9]+\:[0-9]+)";
    const char *src = s.c_str();
    
    common::TimeCalcer tc;
    
    regcomp(&reg, pattern, REG_EXTENDED);
    int status = regexec(&reg, src, nmatch, pmatch, 0);

    if (status == REG_NOMATCH) {
        LOG(INFO) << "NO MATCH";
    } else if (status == 0) {
        std::string res = "";
        for (int i = pmatch[0].rm_so; i < pmatch[0].rm_eo; i++) {
            res += src[i];
        }
        double tm = tc.Calc();
        LOG(INFO) << std::fixed << "cost " << tm;
        LOG(INFO) << res;
    }

    regfree(&reg);
}

int main (int argc, char *argv[]) {
    google::SetStderrLogging(google::GLOG_FATAL);
    FLAGS_logbufsecs = 0;
    ::testing::InitGoogleTest(&argc, argv);
    RUN_ALL_TESTS();
    return 0;
}
