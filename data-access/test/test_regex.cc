#include "gtest/gtest.h"
#include "gflags/gflags.h"
#include "glog/logging.h"
#include <string>
#include <regex>

std::string testnotice = "NOTICE: 17-06-22 16:46:47 errno[0] logId[3661423901] uri[/aese/index.php/api/search] user[127.0.0.1  ] refer[http://www.baidu.com]  cluster[nj03] tm_anti[0.02] baiduid[] cookieid[97F6589AC87B6D9EA78F81C92DEEE3DA] query[东营天气预报] ps_sid[] wise_sid[] sid[104493,100185,116336,104381,117189,116694,107317,117270,116810,117235,117119,117086,116996,117171,116992,117136,116309,115545,116689,116892,116387,115350,114276,115136,116412,116522,110085,117027,100458] net_type[1] mobile_browser[qq] mobile_browser_id[2] mobile_browser_version[6.3] srcid[4186] city_name[东营] province_name[山东] dspName[iphone] dsp[] tm_load_conf[0.22] crd[] tm_rewrite_group[0.03] So_ChangjingStrategy[0_pm | 0_mq | -1_mq] tm_stage_func[0.08 | 0.06 | 0.15] tm_rewrite_cardid[0.2] rw_cards[47033,47034,46019] tm_rewrite_srcida[0.21] make_query[0.09] make_query_fail[47034_17990_make_queryxiaoqi_make_fun] tm_gen_para[0.01] ral_req[So_GssSearch_东营天气预报_wisexmlnew_17990] tm_add_req_m[0.96] tm_add_req[1.11] null_res[So_GssSearch$$$gssSo_GssSearch东营天气预报wisexmlnew$$$47034_17990_ori_query:17990] exv[So_GssSearch$$$gssSo_GssSearch东营天气预报wisexmlnew$$$47034_17990_ori_query:17990] tm_get_res[18.1] ral_res[] tm_merge_res[0.01] tm_group_srcid[0.26] tm_get_card_res[19.23] tm_do_search[20] errmes[null cardid reuslt for main cardid 47034] resno[9] qid[12286720758457292737]";

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

}

TEST(test_regex, test_warning) {
    std::regex reg("\\w+");
}

int main (int argc, char *argv[]) {
    google::SetStderrLogging(google::GLOG_FATAL);
    FLAGS_logbufsecs = 0;
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
