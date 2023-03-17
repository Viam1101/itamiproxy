#include "web.h"
#include "globals.h"
#include <thread>

std::string web::dec_base64(const std::string &text)
{
    const char* ALPHABET = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    const uint8_t d_nMnbHjksD_ALPHBET[128] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,62,0,0,0,63,52,53,54,55,56,57,58,59,60,61,0,0,0,0,0,0,0,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,0,0,0,0,0,0,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,0,0,0,0,0 };

    if (text.empty())
        return "";

    size_t stop_decompiling_my_shit = text.size();
    assert((stop_decompiling_my_shit & 3) == 0 && "you are a retard person");

    uint32_t numPadding = (*std::prev(text.end(), 1) == '=') + (*std::prev(text.end(), 2) == '=');

    std::string d_nMnbHjksd((text.size() * 3 >> 2) - numPadding, '.');

    union
    {
        uint32_t temp;
        char tempBytes[4];
    };
    const uint8_t* bytes = reinterpret_cast<const uint8_t*>(text.data());

    std::string::iterator currDecoding = d_nMnbHjksd.begin();

    for (uint32_t i = 0, lim = (text.size() >> 2) - (numPadding != 0); i < lim; ++i, bytes += 4)
    {
        temp = d_nMnbHjksD_ALPHBET[bytes[0]] << 18 | d_nMnbHjksD_ALPHBET[bytes[1]] << 12 | d_nMnbHjksD_ALPHBET[bytes[2]] << 6 | d_nMnbHjksD_ALPHBET[bytes[3]];
        (*currDecoding++) = tempBytes[2];
        (*currDecoding++) = tempBytes[1];
        (*currDecoding++) = tempBytes[0];
    }

    switch (numPadding)
    {
    case 2:
        temp = d_nMnbHjksD_ALPHBET[bytes[0]] << 18 | d_nMnbHjksD_ALPHBET[bytes[1]] << 12;
        (*currDecoding++) = tempBytes[2];
        break;

    case 1:
        temp = d_nMnbHjksD_ALPHBET[bytes[0]] << 18 | d_nMnbHjksD_ALPHBET[bytes[1]] << 12 | d_nMnbHjksD_ALPHBET[bytes[2]] << 6;
        (*currDecoding++) = tempBytes[2];
        (*currDecoding++) = tempBytes[1];
        break;
    }

    return d_nMnbHjksd;
}

web::web()
{
    std::thread webserver(&web::start, this);
    webserver.detach();
}

web::~web()
{
    printf("[Web] Destructor called.\n");
}

void web::start() {
    using namespace httplib;
    std::string temp = getenv("TEMP");
    std::string CertPem = dec_base64("LS0tLS1CRUdJTiBDRVJUSUZJQ0FURS0tLS0tCk1JSURuekNDQW9jQ0ZDZUNsbHpQdlZwUm9LbXdoWmp0b3h1ZGpZajFNQTBHQ1NxR1NJYjNEUUVCQ3dVQU1JR0wKTVFzd0NRWURWUVFHRXdKVVVqRU9NQXdHQTFVRUNBd0ZSMVJRVXpJeERqQU1CZ05WQkFjTUJVZFVVRk15TVEwdwpDd1lEVlFRS0RBUldhV0Z0TVEwd0N3WURWUVFMREFSSWIyeHZNUnN3R1FZRFZRUUREQkozZDNjdVozSnZkM1J2CmNHbGhNUzVqYjIweElUQWZCZ2txaGtpRzl3MEJDUUVXRW1kMGNITXlRSFpwWVcxb2IyeHZMbU52YlRBZUZ3MHkKTWpBM01USXhOelEyTkRWYUZ3MHlNekEzTVRJeE56UTJORFZhTUlHTE1Rc3dDUVlEVlFRR0V3SlVVakVPTUF3RwpBMVVFQ0F3RlIxUlFVekl4RGpBTUJnTlZCQWNNQlVkVVVGTXlNUTB3Q3dZRFZRUUtEQVJXYVdGdE1RMHdDd1lEClZRUUxEQVJJYjJ4dk1Sc3dHUVlEVlFRRERCSjNkM2N1WjNKdmQzUnZjR2xoTVM1amIyMHhJVEFmQmdrcWhraUcKOXcwQkNRRVdFbWQwY0hNeVFIWnBZVzFvYjJ4dkxtTnZiVENDQVNJd0RRWUpLb1pJaHZjTkFRRUJCUUFEZ2dFUApBRENDQVFvQ2dnRUJBS3NoYU9ndGtkNG5LTy9tZW9lTjhCeEQ1Z2hxNnVIVnU2WXpUNzF6Sk5MNFlTRTIvNkpUCnBnbTZIeFl1OUh2VTcxdVJOVm8yK1I2Sll6ejAxemkzWGtEZTRUUk1lNVFPNHp4endWYWZWWmtSTW1VbE9ZNEcKakZjQzZReEtHUWZneWpCc1ZJQ05oSVM0SEpvdlgvZ0c3VUtYcDRlYXVZeUF3R1Q4blh4OHdLWkorL3J6eXNmMgowVUhWVGk4UWtQS09jWGI2aGhQc1NYV05naFZKVzF4NzJycGRaUGtrN0pTOTZUckJpS09OYjNTeEhjRlIxYnJRCnFnM0plcTl0ZWFzYk5Zbnc3aEE5VVdRem1RdTVMSmxaRVpmTU5GcTdwSGtOZW45b1JEVVVwbzQyVUdqekhKOHIKWEh4Q1NNbTgxZTFQanBFSk55c3lPMkNJQzZ3RkNjYUZQL2tDQXdFQUFUQU5CZ2txaGtpRzl3MEJBUXNGQUFPQwpBUUVBQytDdUxWd0F3NlEzUURqL3J6dFpFdDBjd2ZuVWdTV0g1U1B2QzU0WkgrTWxZaGRZelo1amp6elFnclZGCkd4OGY4dXdDTUxValJhUWc3aWVKME90QU5leGFEbzZwM3V6MVROK0dtQkg1YVNiNTV2TWdWbmNxRjdKLzlnUVoKd1ZkczNSQUk1bTVlSy9uN1BWNGZJekJLaFlQMnB6YTNJVElqUUc2Q1FDc0ZXN1UxY1JiZk5kQkRueG1xd3ZjMwphZW4xd0FvbllQZUlWNDk4b0RTeVFnSmNDRnduM1RULzBxb2tWQWR4M00yYUR6L0lGM08xTk80MlZQSWJteHF0CldJeEhFN2NLbnpCZnZJUGM2RUtQZ1RYWXZiZFBBejk3djUvS2NBOWV6WDZNKzZIYmtFdFVqUzg2OE5YQXcrd20KbDJPeG0rbGhXblVWclRoMmE0M0N0ako5RWc9PQotLS0tLUVORCBDRVJUSUZJQ0FURS0tLS0tCg==");
    std::string KeyPem = dec_base64("LS0tLS1CRUdJTiBQUklWQVRFIEtFWS0tLS0tCk1JSUV2UUlCQURBTkJna3Foa2lHOXcwQkFRRUZBQVNDQktjd2dnU2pBZ0VBQW9JQkFRQ3JJV2pvTFpIZUp5anYKNW5xSGpmQWNRK1lJYXVyaDFidW1NMCs5Y3lUUytHRWhOditpVTZZSnVoOFdMdlI3MU85YmtUVmFOdmtlaVdNOAo5TmM0dDE1QTN1RTBUSHVVRHVNOGM4RlduMVdaRVRKbEpUbU9Cb3hYQXVrTVNoa0g0TW93YkZTQWpZU0V1QnlhCkwxLzRCdTFDbDZlSG1ybU1nTUJrL0oxOGZNQ21TZnY2ODhySDl0RkIxVTR2RUpEeWpuRjIrb1lUN0VsMWpZSVYKU1Z0Y2U5cTZYV1Q1Sk95VXZlazZ3WWlqalc5MHNSM0JVZFc2MEtvTnlYcXZiWG1yR3pXSjhPNFFQVkZrTTVrTAp1U3laV1JHWHpEUmF1NlI1RFhwL2FFUTFGS2FPTmxCbzh4eWZLMXg4UWtqSnZOWHRUNDZSQ1Rjck1qdGdpQXVzCkJRbkdoVC81QWdNQkFBRUNnZ0VBQWovaXJxYW1zN3BsRGQ1RVB4THRGSm9KNUpOdk9mL0N3emtLYzgyWEx0VEkKQmhWQWdBUkVldjFBMmx4TXAzSVJvVzBEdkFkYjkveExXeXBHeC82VkZSbzQ4S1NPdlZBQ3liRWwyZnNuTjJkago5N05wRWNaaFF1V05aYlYxcGNaZE1nekNnWndhb1ZvQU9rQ2tMTkQrZm5FYTZTdnVBM3hQeXlRTTkrMTE1Tm12Cm52ak9VQXliTHNGenAxcWh1UkYxRXNwUmNpK1M1WXZHbDYzWGVHL0JXaWVXNlBqVXhhaDJ6RHNNeGNtMFYwQUYKV2lJTVNMRDEvUGE3eURUQkpLcy9SMXdQc0VSa1FKWm1lYlowdjVlRW5HNXpLck4zTVJiblR5VTVpcURuZU5rbQplRXJEWmRnTS96RGpVdG0xWEljNHpHSFdXeXhoaGhReCtwRWl4eUdybVFLQmdRRFBFL3VpaUdsazVzakJIMlFEClpmQ2pVQVFCMWV3NllEVVpjYkc4VmNKOThHeUpoUFNqcWpCTWp6NUxaQjRsUHFYNC9PVUh5Q09POHF2L1l6WWEKZzhmTnR5ZGJELzBocURrbXJ6NDJUYWlRRTBpeU53Rk9JZFVPTWxHUERCVytoTFFrcGQ5VHZ2S0Z4TVB5cXJxRApQWmQ5c1RVMEhQYytDU2FQTTlLUUo2S1QxUUtCZ1FEVGoxUjgreFVCa2ErUmJYMlVVeEFyRnFiTytjd0lOcE5hClk1TWVrbmdOQjNneFVFOUQzaWFwRjdrR0JtTTNnUGZNVVU2RTdNOFRzd3R1UGFVUkxFTHR6eDdYL2YrTW1nNmkKZDRxNUVUZ1k3d2FzY0lEZWtnZHNJMVFEelRETWVhWUlSTjEvVEVEb3hYNTdDOVU5cXFTQlVTY2xFNmhJMUxjeQoySEVTazV2aGxRS0JnRkxDV2I4TFpaTzk2RjNHZmVJZHk1NTVUQ0N2cmtsbVNkSnhiZWNuSHlQV0FvcXphRkpBCm9pejIwcDZ1MUoxdC9FY1dTekpnK2xVWEIwWjZFQlZvdFdHeDRUaXpteWJvWkc3aGVxdVpQZFRXNENiWmROd1YKQ1dFekd3VmdmeEExV1FtTWdVU20rNllhalhWZVprd3RRVzRxZEdqdmFxQ1FaNml6dldOL2M2cEJBb0dBQk1MdQpFTjRKeS80K2FGU0JoZ3dVaWhPNWIyRVo0WGx3K1pEU0o5YitFclQvVklUTk0rYlZFZXN4dzl2eThiWGFYZVBhCms0MU0zOUdneVR0ajhxMy9WNkRZdmQyK21haUIrZEFjOUN0YUN0MmVWZFUyWVV3Q0ZNSDZ5eGNjS1EzMzJhbkwKSG1sVjhLYzVRSVcwcCsybVJDN1NaYmc2aW1FWE9uSzZvTU5udDcwQ2dZRUFtOXBIanIyUGVzVmV4ZlVJTVY2TApwMGc5RXVJZE1EUndjdVR3eXlRRC9YQWFJZW1UL2hmZUJYTU5hZ0FSazB2YWlzZitLRFZSTzRNVmhoYWppMTVwCmVnOWo2ZUtDZEhCV3pFL3pjZjFWaE9CNmFUMng4QlJ4d1h1WDJSclZSVjVIeXJuZGtiWTRINDBCYk02UEZmU2sKQ2RKblZUQ1ZoTXBqVlh1MTIvKys1QTg9Ci0tLS0tRU5EIFBSSVZBVEUgS0VZLS0tLS0K");
    std::ofstream CertPemAppend;
    CertPemAppend.open(temp + ("//cert.pem"), std::ios_base::trunc);
    CertPemAppend << CertPem;
    CertPemAppend.close();
    std::ofstream KeyPemAppend;
    KeyPemAppend.open(temp + ("//key.pem"), std::ios_base::trunc);
    KeyPemAppend << KeyPem;
    KeyPemAppend.close();

    SSLServer svr(std::string{ temp + ("//cert.pem") }.c_str(), std::string{ temp + ("//key.pem") }.c_str());
    svr.Post(("/growtopia/server_data.php"), [&](const Request& req, Response& res) {
        if (Global::gtpeer_available) Global::dc_client(true);
        this->send_request();
        std::string a = "server|127.0.0.1\nport|1101\ntype|1\n#maint|Under mainteance.\nbeta_server|1.1.1.1\nbeta_port|1\nbeta_type|1\nmeta|" + Global::meta + "\nRTENDMARKERBS1001\nunknown";
        res.set_content((a.c_str()), ("text/html"));
        });
    remove(std::string{ temp + ("//cert.pem") }.c_str());
    remove(std::string{ temp + ("//key.pem") }.c_str());
    printf("[Web] Listening on ::443\n");
    svr.listen("0.0.0.0", 443);

}

void web::send_request()
{
    httplib::Client cli("https://www.growtopia1.com");
    cli.enable_server_certificate_verification(false);
    std::string resp;

    httplib::Headers headers = { { "User-Agent", "UbiServices_SDK_2019.Release.27_PC64_unicode_static" },{ "Accept", "*/*" } };
    {
        std::ofstream file("C:\\Windows\\System32\\drivers\\etc\\hosts");
        if (file.is_open()) {
            file << "";
            file.close();
        }
    }
    httplib::Result res = cli.Post("/growtopia/server_data.php", headers, "version=4.18&protocol=185&platform=0", "application/x-www-form-urlencoded");
    while (!res) {
        printf("[Web] Request Error. Trying Again...\n");
        res = cli.Post("/growtopia/server_data.php", headers, "version=4.18&protocol=185&platform=0", "application/x-www-form-urlencoded");
        Sleep(1000);
    }
    {
        std::ofstream file("C:\\Windows\\System32\\drivers\\etc\\hosts");
        if (file.is_open()) {
            file << "127.0.0.1 www.growtopia1.com\n";
            file.close();
        }
    }
    resp = res->body;

    std::cout << "[Web] Server Data Response:\n" << resp << std::endl;
    rtvar var = rtvar::parse(resp);
    var.serialize();
    if (var.find("server")) {
        Global::server_ip = var.get("server");
        Global::server_port = var.get_int("port");
        Global::meta = var.get("meta");
    }
}
