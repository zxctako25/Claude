#include "modelselectordialog.h"
#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QFrame>
#include <QListWidget>
#include <QGraphicsDropShadowEffect>
#include <QPainter>
#include <QPixmap>
#include <QSvgRenderer>
#include <QScrollBar>
#include <QTimer>
#include <QScreen>
#include <QGuiApplication>
#include <QDebug>

static QString svgForFamily(const QString &family)
{
    // Anthropic
    if (family == "Haiku" || family == "Sonnet"
        || family == "Opus"  || family == "Fable")
    {
        return R"(<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 36 36">
            <path fill="currentColor" d="M26.9568 9.88184H22.1265L30.7753 31.7848H35.4917L26.9568 9.88184ZM13.028 9.88184L4.4917 31.7848H9.32203L11.2305 27.1793H20.2166L22.0126 31.6724H26.8444L18.0832 9.88184H13.028ZM12.5783 23.1361L15.4987 15.3853L18.5315 23.1361H12.5783Z"/>
        </svg>)";
    }

    // OpenAI
    if (family == "GPT")
    {
        return R"(<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 36 36">
            <path fill="currentColor" d="M32.8377 17.282C33.2127 16.25 33.3072 15.218 33.2127 14.1875C33.1197 13.1571 32.7447 12.1251 32.2752 11.1876C31.4322 9.78209 30.2127 8.6571 28.8072 8.0001C27.3072 7.34461 25.7127 7.15711 24.1197 7.53211C23.3698 6.78212 22.5253 6.12512 21.5878 5.65713C20.6503 5.18913 19.5253 5.00013 18.4948 5.00013C16.8851 4.99074 15.3125 5.48246 13.9948 6.40712C12.6824 7.34311 11.7449 8.6571 11.2754 10.1571C10.1504 10.4376 9.21289 10.9071 8.27539 11.4696C7.4324 12.1251 6.77541 12.9696 6.21291 13.8126C5.36992 15.2195 5.08792 16.8125 5.27542 18.407C5.46399 19.9968 6.11605 21.496 7.1504 22.718C6.79608 23.7086 6.66795 24.7659 6.77541 25.8124C6.86991 26.8444 7.2449 27.8749 7.7129 28.8124C8.55739 30.2194 9.77538 31.3444 11.1824 31.9999C12.6824 32.6569 14.2753 32.8444 15.8698 32.4694C16.6198 33.2194 17.4628 33.8749 18.4003 34.3444C19.3378 34.8139 20.4628 34.9999 21.4948 34.9999C23.1043 35.0097 24.6769 34.5185 25.9947 33.5944C27.3072 32.6569 28.2447 31.3444 28.7127 29.8444C29.7719 29.6432 30.7682 29.1934 31.6197 28.5319C32.4627 27.8749 33.2127 27.1249 33.6822 26.1874C34.5251 24.7819 34.8071 23.1875 34.6196 21.5945C34.4322 20 33.8697 18.5015 32.8377 17.282ZM21.5878 33.0304C20.0878 33.0304 18.9628 32.5609 17.9323 31.7179C17.9323 31.7179 18.0253 31.6234 18.1198 31.6234L24.1197 28.1554C24.2862 28.0803 24.4196 27.9469 24.4947 27.7804C24.5698 27.636 24.6021 27.4731 24.5877 27.3109V18.875L27.1197 20.375V27.3124C27.1455 28.0547 27.0215 28.7945 26.755 29.4878C26.4885 30.181 26.085 30.8134 25.5687 31.3473C25.0523 31.8811 24.4337 32.3054 23.7497 32.5949C23.0658 32.8843 22.3305 33.0314 21.5878 33.0304ZM9.49488 27.8749C8.83789 26.7499 8.55739 25.4374 8.83789 24.125C8.83789 24.125 8.93239 24.2195 9.02539 24.2195L15.0253 27.6874C15.1693 27.7638 15.3325 27.7966 15.4948 27.7819C15.6823 27.7819 15.8698 27.7819 15.9628 27.6874L23.2753 23.4695V26.3749L17.1823 29.9374C16.5506 30.3042 15.8527 30.5427 15.1287 30.6393C14.4046 30.7358 13.6686 30.6884 12.9629 30.4999C11.4629 30.1249 10.2449 29.1874 9.49488 27.8749ZM7.9004 14.8445C8.56239 13.7234 9.58826 12.8627 10.8074 12.4056V19.532C10.8074 19.718 10.8074 19.907 10.9004 20C10.9755 20.1665 11.1089 20.2998 11.2754 20.375L18.5878 24.5944L16.0573 26.0944L10.0574 22.625C9.41842 22.2639 8.85742 21.7797 8.40684 21.2004C7.95627 20.6211 7.62506 19.9582 7.4324 19.25C7.05741 17.8445 7.1504 16.157 7.9004 14.8445ZM28.6197 19.625L21.3073 15.407L23.8377 13.9071L29.8377 17.375C30.7752 17.9375 31.5252 18.6875 31.9947 19.625C32.4642 20.5625 32.7447 21.5945 32.6502 22.7195C32.5603 23.7755 32.1699 24.7837 31.5252 25.6249C30.8697 26.4694 30.0252 27.1249 28.9947 27.4999V20.375C28.9947 20.1875 28.9947 20 28.9002 19.907C28.9002 19.907 28.8072 19.718 28.6197 19.625ZM31.1502 15.875C31.1502 15.875 31.0572 15.782 30.9627 15.782L24.9627 12.3126C24.7752 12.2196 24.6822 12.2196 24.4947 12.2196C24.3072 12.2196 24.1197 12.2196 24.0252 12.3126L16.7128 16.532V13.6251L22.8073 10.0626C23.7448 9.50009 24.7752 9.31259 25.9002 9.31259C26.9322 9.31259 27.9627 9.68759 28.9002 10.3446C29.7447 11.0001 30.4947 11.8446 30.8697 12.7821C31.2447 13.7196 31.3377 14.8445 31.1502 15.875ZM15.4003 21.125L12.8699 19.625V12.5946C12.8699 11.5626 13.1503 10.4376 13.7128 9.59459C14.2753 8.6571 15.1198 8.0001 16.0573 7.53211C17.0127 7.05249 18.0956 6.88812 19.1503 7.06261C20.1823 7.15711 21.2128 7.62511 22.0573 8.2821C22.0573 8.2821 21.9628 8.3751 21.8698 8.3751L15.8698 11.8446C15.7033 11.9197 15.57 12.0531 15.4948 12.2196C15.4003 12.4071 15.4003 12.5001 15.4003 12.6876V21.125ZM16.7128 18.125L19.9948 16.25L23.2753 18.125V21.875L19.9948 23.75L16.7128 21.875V18.125Z"/>
        </svg>)";
    }

    // DeepSeek
    if (family == "DeepSeek")
    {
        return R"(<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 36 36">
            <path fill="currentColor" d="M35.6638 9.91965C35.3251 9.75432 35.1785 10.0703 34.9811 10.2316C34.9131 10.2836 34.8558 10.3516 34.7985 10.413C34.3025 10.9423 33.7238 11.289 32.9678 11.2476C31.8625 11.1863 30.9186 11.533 30.0839 12.3783C29.9066 11.3356 29.3173 10.7143 28.4213 10.3143C27.9519 10.1063 27.4773 9.89965 27.148 9.44766C26.9186 9.12633 26.856 8.76767 26.7413 8.41568C26.668 8.20235 26.5946 7.98502 26.3506 7.94902C26.084 7.90769 25.98 8.13035 25.876 8.31702C25.4587 9.07967 25.2973 9.91965 25.3133 10.7703C25.3493 12.6849 26.1573 14.2102 27.764 15.2942C27.9466 15.4182 27.9933 15.5435 27.9359 15.7249C27.8266 16.0982 27.696 16.4609 27.5813 16.8355C27.508 17.0742 27.3986 17.1248 27.1426 17.0222C26.2777 16.6504 25.4919 16.1164 24.828 15.4489C23.6854 14.3449 22.6534 13.1263 21.3654 12.1716C21.067 11.9511 20.7606 11.7416 20.4468 11.5436C19.1335 10.2676 20.6201 9.21967 20.9641 9.09567C21.3241 8.965 21.0881 8.51968 19.9254 8.52501C18.7628 8.53035 17.6988 8.91834 16.3428 9.43699C16.1413 9.51421 15.934 9.57529 15.7229 9.61966C14.4557 9.38091 13.1598 9.33506 11.8789 9.48366C9.36565 9.76365 7.35902 10.953 5.88305 12.9809C4.10975 15.4182 3.69243 18.1888 4.20308 21.0768C4.74041 24.122 6.29504 26.6433 8.683 28.6139C11.1603 30.6579 14.0122 31.6592 17.2668 31.4672C19.2428 31.3539 21.4441 31.0886 23.9254 28.9873C24.552 29.2993 25.208 29.4233 26.2986 29.5166C27.1386 29.5953 27.9466 29.4766 28.5719 29.3459C29.5519 29.1379 29.4839 28.23 29.1306 28.0646C26.2573 26.726 26.888 27.2713 26.3133 26.83C27.7746 25.102 29.9746 23.3074 30.8359 17.4928C30.9026 17.0302 30.8452 16.7395 30.8359 16.3662C30.8306 16.1395 30.8826 16.0502 31.1426 16.0249C31.8639 15.95 32.5637 15.7349 33.2025 15.3915C35.0638 14.3742 35.8158 12.7049 35.9931 10.7023C36.0198 10.3956 35.9878 10.081 35.6638 9.91965ZM19.4414 27.9433C16.6562 25.754 15.3055 25.0327 14.7482 25.0634C14.2256 25.0954 14.3202 25.6913 14.4349 26.0807C14.5549 26.4647 14.7109 26.7286 14.9295 27.066C15.0815 27.2886 15.1855 27.6206 14.7789 27.87C13.8816 28.4246 12.3229 27.6833 12.2496 27.6473C10.435 26.578 8.91632 25.1673 7.84834 23.2381C6.81637 21.3808 6.21638 19.3888 6.11771 17.2622C6.09105 16.7475 6.24171 16.5662 6.7537 16.4729C7.42583 16.3442 8.11451 16.3267 8.79233 16.4209C11.6349 16.8368 14.0536 18.1075 16.0828 20.1194C17.2402 21.2661 18.1161 22.6354 19.0188 23.974C19.9788 25.3953 21.0108 26.75 22.3254 27.8593C22.7894 28.2486 23.1587 28.5446 23.5134 28.7619C22.4441 28.8819 20.6601 28.9086 19.4414 27.9433ZM20.7748 19.3568C20.7745 19.2906 20.7904 19.2253 20.8211 19.1666C20.8517 19.1078 20.8962 19.0575 20.9507 19.0198C21.0052 18.9821 21.068 18.9583 21.1337 18.9503C21.1995 18.9424 21.2662 18.9505 21.3281 18.9741C21.407 19.0024 21.475 19.0546 21.5228 19.1235C21.5706 19.1923 21.5958 19.2743 21.5947 19.3581C21.5949 19.4123 21.5843 19.4659 21.5636 19.5159C21.5428 19.5659 21.5123 19.6113 21.4738 19.6494C21.4354 19.6875 21.3897 19.7176 21.3395 19.7378C21.2893 19.7581 21.2356 19.7682 21.1814 19.7675C21.1277 19.7676 21.0745 19.7571 21.0248 19.7365C20.9752 19.7158 20.9302 19.6855 20.8925 19.6473C20.8548 19.609 20.825 19.5636 20.805 19.5138C20.785 19.4639 20.7739 19.4105 20.7748 19.3568ZM24.9213 21.4848C24.6547 21.5928 24.3893 21.6861 24.1347 21.6981C23.7516 21.7114 23.3756 21.5918 23.0707 21.3594C22.7054 21.0528 22.4441 20.8821 22.3347 20.3488C22.297 20.0881 22.3042 19.823 22.3561 19.5648C22.4494 19.1288 22.3454 18.8488 22.0374 18.5955C21.7881 18.3875 21.4694 18.3302 21.1201 18.3302C21.0005 18.3232 20.8843 18.2875 20.7814 18.2262C20.6348 18.1542 20.5148 17.9728 20.6294 17.7488C20.6668 17.6768 20.8428 17.5008 20.8854 17.4688C21.3601 17.1995 21.9081 17.2875 22.4134 17.4902C22.8827 17.6822 23.2374 18.0342 23.748 18.5328C24.2694 19.1341 24.364 19.3008 24.6613 19.7515C24.896 20.1048 25.1093 20.4674 25.2547 20.8821C25.344 21.1421 25.2293 21.3541 24.9213 21.4848Z"/>
        </svg>)";
    }

    // GLM
    if (family == "GLM")
    {
        return R"(<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 36 36">
            <path fill="currentColor" d="M20.1312 7.50002L17.4088 11.1913H5.81625L8.5375 7.50002H20.1325H20.1312ZM34.0675 28.81L31.3475 32.5H19.795L22.5125 28.81H34.0675ZM35 7.50002L16.58 32.5H5L23.42 7.50002H35Z"/>
        </svg>)";
    }

    // Grok
    if (family == "Grok")
    {
        return R"(<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 36 36">
            <path fill="currentColor" d="M12.4579 15.6036L26.1529 35H20.0656L6.37059 15.6036H12.4579ZM12.4524 26.3764L15.4974 30.6909L12.4551 35H6.36377L12.4524 26.3764ZM33.6365 7.15727V35H28.647V14.2236L33.6365 7.15727ZM33.6365 5L20.0656 24.2205L17.0206 19.9073L27.5451 5H33.6365Z"/>
        </svg>)";
    }

    // Kimi
    if (family == "Kimi")
    {
        return R"(<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 36 36">
            <path fill="currentColor" d="M5.85893 26.3499L18.1801 29.6463C18.1635 30.5223 18.1894 31.3987 18.2576 32.2722L25.9507 34.3298C23.669 35.2716 21.1952 35.6547 18.7355 35.4471L18.503 35.4265L18.4462 35.4213L18.3377 35.4097L18.2163 35.3968C18.1486 35.3886 18.081 35.38 18.0135 35.3709L17.8753 35.3528L17.7332 35.3322C17.5951 35.3121 17.4573 35.2901 17.3198 35.2663L17.2656 35.256L17.1687 35.2392L17.0305 35.2133L16.9401 35.194L16.82 35.1694L16.7231 35.1488L16.6004 35.1229L16.4751 35.0932L16.3537 35.0648L16.2659 35.0428L16.1522 35.0144L16.0359 34.9834L15.9132 34.9511L15.8073 34.9214L15.6665 34.8827L15.5864 34.8568L15.4779 34.8245L15.3578 34.7884L15.2222 34.7445L15.1473 34.7199L15.0439 34.6863L14.9277 34.6463L14.8424 34.6153C14.8235 34.6089 14.8045 34.6025 14.7856 34.5959L14.6978 34.5636L14.5673 34.5158L14.4937 34.4874L14.3904 34.4487L14.278 34.4035L14.1643 34.3583L14.0623 34.3169L13.9396 34.2653L13.8582 34.2291L13.7768 34.1942C13.7591 34.1865 13.7415 34.1788 13.7239 34.171L13.6386 34.1322L13.5056 34.0715L13.4384 34.0405L13.3144 33.9811L13.2343 33.9423L13.1258 33.8907L13.0147 33.8338L12.8946 33.7731L12.8274 33.7383L12.6944 33.6672L12.6208 33.6285L12.5459 33.5871C12.526 33.576 12.5062 33.5648 12.4864 33.5536L12.365 33.4851L12.2875 33.4412L12.2217 33.4024L12.1287 33.3495L12.0227 33.2849L11.9026 33.2126L11.8354 33.1712L11.7269 33.1028L11.6482 33.0524L11.5461 32.9878L11.4557 32.9271L11.3872 32.8819C11.3639 32.8665 11.3407 32.851 11.3175 32.8354L11.2607 32.7966L11.2038 32.7579C11.1866 32.7459 11.1693 32.7338 11.1522 32.7217L11.0785 32.6701L10.9804 32.6003L10.8912 32.5357L10.7957 32.466L10.7233 32.4117L10.6252 32.3381L10.527 32.2619L10.4159 32.1753L10.3578 32.1301L10.2751 32.063L10.1795 31.9855L10.0646 31.8912L10.0052 31.8408L9.94575 31.7904C9.92717 31.7746 9.90866 31.7586 9.8902 31.7426L9.83208 31.691L9.75329 31.6225L9.66287 31.5424L9.57504 31.4649L9.49496 31.39L9.40841 31.3099L9.33996 31.2453L9.22629 31.1368C9.18345 31.0953 9.14082 31.0535 9.09842 31.0116L9.06096 30.9754L9.008 30.9211L8.91887 30.8307L8.85429 30.7648L8.78971 30.6964C8.71592 30.6207 8.64357 30.5436 8.57271 30.4652L8.46938 30.3515L8.38929 30.2611L8.29758 30.1578L8.24333 30.0945L8.17488 30.0144L8.09996 29.9266L8.04054 29.8542C8.02888 29.8401 8.01725 29.8258 8.00567 29.8116L7.94754 29.7406L7.86229 29.6346L7.80934 29.5675L7.74475 29.4848L7.71892 29.4525C6.97985 28.4945 6.3556 27.4532 5.85893 26.3499Z"/>
        </svg>)";
    }

    // Qwen
    if (family == "Qwen")
    {
        return R"(<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 36 36">
            <path fill="currentColor" d="M37.9998 23.021C33.7998 25.2889 29.5698 27.3649 24.8614 28.3069C23.8114 28.5154 22.6474 28.5154 21.5809 28.3714C20.5639 28.2439 20.0554 27.3484 20.4169 26.4064C20.7619 25.5289 21.2209 24.635 21.8119 23.9C23.0899 22.3025 24.5329 20.849 25.8289 19.268C26.6203 18.2991 27.3335 17.2689 27.9618 16.187C28.4208 15.4205 28.2078 14.4935 27.4038 14.111C26.0584 13.4556 24.6154 12.9936 23.1889 12.4986C23.0239 12.4341 22.7779 12.6096 22.4509 12.7221C22.8604 13.0881 23.1559 13.3596 23.5654 13.727C19.3339 14.447 15.3305 15.467 11.4455 16.874C11.4275 16.9535 11.396 17.0165 11.411 17.0495C11.9855 17.927 11.723 18.5975 10.886 19.1405C10.5611 19.3531 10.2732 19.6176 10.034 19.9235C12.593 20.6735 14.873 20.243 17.0539 18.821C16.9234 18.6305 16.7914 18.455 16.6609 18.263C17.4799 18.407 17.9719 18.854 18.0379 19.556C18.0544 19.7165 17.9569 19.8755 17.9074 20.036C17.7919 19.907 17.6449 19.781 17.5474 19.6355C17.4799 19.5395 17.4634 19.4285 17.4154 19.268C14.8235 20.993 12.035 21.425 8.96751 20.531C8.96751 21.137 8.93451 21.6485 8.98401 22.1435C9.01701 22.574 8.83701 22.766 8.44401 22.9895C7.55752 23.5325 6.63803 24.092 5.90003 24.8105C5.01504 25.6879 5.34354 26.7589 6.54053 27.2059C7.90102 27.7159 9.329 27.7309 10.7555 27.5569C12.4445 27.3484 14.1005 27.0769 15.9394 26.8219C13.79 27.8269 11.6735 28.5319 9.4445 28.8169C7.88452 29.0269 6.32753 29.1379 4.78554 28.6909C2.57156 28.0684 1.58607 26.4394 2.16057 24.251C2.70206 22.2065 4.01455 20.5775 5.42454 19.076C10.133 14.078 16.0864 11.5401 22.9744 11.0286C24.5824 10.9176 26.2069 11.1246 27.7143 11.7951C29.8308 12.7536 30.7173 14.78 29.6838 16.826C29.0118 18.1835 28.0758 19.4285 27.1413 20.6585C26.2234 21.872 25.1899 22.9895 24.2224 24.155C23.9434 24.506 23.6809 24.875 23.4679 25.2724C23.0569 26.0224 23.3359 26.5174 24.2059 26.4394C26.0254 26.2624 27.8808 26.1199 29.6358 25.6729C32.2098 25.0174 34.7193 24.092 37.2618 23.2775C37.5243 23.213 37.7703 23.117 37.9998 23.0225V23.021Z"/>
        </svg>)";
    }

    // Gemini
    if (family == "Gemini")
    {
        return R"(<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 40 40">
            <path fill="currentColor" d="M37 20.034C27.8809 20.5837 20.5808 27.8809 20.0326 37H19.966C19.4163 27.8809 12.1177 20.5837 3 20.034V19.9674C12.1191 19.4163 19.4163 12.1191 19.966 3H20.0326C20.5822 12.1191 27.8809 19.4163 37 19.9674V20.034Z"/>
        </svg>)";
    }

    return R"(<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 36 36">
        <circle cx="18" cy="18" r="14" fill="none" stroke="currentColor" stroke-width="2"/>
    </svg>)";
}

static QPixmap renderSvg(const QString &svg, int size, const QColor &color)
{
    const int ss = 4;
    const int bigSize = size * ss;

    QImage big(bigSize, bigSize, QImage::Format_ARGB32_Premultiplied);
    big.fill(Qt::transparent);

    QString colored = svg;
    colored.replace("currentColor", color.name());

    QSvgRenderer r(colored.toUtf8());
    if (!r.isValid())
    {
        QPixmap empty(size, size);
        empty.fill(Qt::transparent);
        return empty;
    }

    QPainter pBig(&big);
    pBig.setRenderHint(QPainter::Antialiasing, true);
    pBig.setRenderHint(QPainter::SmoothPixmapTransform, true);
    r.render(&pBig);
    pBig.end();

    QImage small = big.scaled(size, size,
                              Qt::KeepAspectRatio,
                              Qt::SmoothTransformation);

    QPixmap pm = QPixmap::fromImage(small);

    if (QGuiApplication::primaryScreen())
    {
        qreal dpr = QGuiApplication::primaryScreen()->devicePixelRatio();
        pm.setDevicePixelRatio(dpr);
    }
    return pm;
}

static QString descriptionForId(const QString &rawId)
{
    QString key = rawId.toLower().trimmed().replace('_', '-');

    // Opus
    if (key == "claude-opus-5")
        return "Новейшая модель";
    if (key == "claude-opus-4.8" || key == "claude-opus-4-8")
        return "Флагман Anthropic — максимум качества на самых трудных задачах";
    if (key == "claude-opus-4.7" || key == "claude-opus-4-7")
        return "Улучшенный Opus: глубже рассуждает, точнее в агентных сценариях";
    if (key == "claude-opus-4.6" || key == "claude-opus-4-6")
        return "Мощная модель для сложных задач и большого кода";

    // Sonnet
    if (key == "claude-sonnet-5" || key == "claude-sonnet-5.0")
        return "Новое поколение Sonnet: быстрее и умнее прежнего";
    if (key == "claude-sonnet-4.6" || key == "claude-sonnet-4-6")
        return "Баланс скорости и ума для повседневной работы";

    // Haiku+Fable
    if (key.contains("haiku"))
        return "Самая быстрая и компактная модель для простых задач";
    if (key.contains("fable"))
        return "Для сложных задач рассуждения, глубокого анализа, длительной агентской работы и программирования";

    // GPT
    if (key == "gpt-5.4")
        return "Надёжная GPT для кода и текста, выгоднее по цене";
    if (key == "gpt-5.5")
        return "Универсальная модель OpenAI для широкого круга задач";
    if (key == "gpt-5.6-luna")
        return "Это самая быстрая и экономичная модель в семействе передовых нейросетей GPT-5.6";
    if (key == "gpt-5.6-sol")
        return "Флагманская модель рассуждений и самая мощная модель от OpenAI на сегодняшний день";
    if (key == "gpt-5.6-terra")
        return "Это сбалансированная мультимодальная модель от OpenAI, занимающая среднее положение в линейке GPT-5.6";
    if (key == "gpt-6-astra")
        return "Флагманская мультимодальная языковая модель от OpenAI, разработанная как универсальный компьютерный агент для автономного выполнения сложных и многочасовых задач";

    // GPT Image
    if (key == "gpt-image-2")
        return "Генерация и редактирование изображений";
    if (key == "gpt-image-2.5")
        return "Генерация и редактирование изображений нового поколения";

    // DeepSeek
    if (key == "deepseek-v4-flash")
        return "Молниеносные ответы на лёгких задачах";
    if (key == "deepseek-v4-pro")
        return "Сильная в коде и математике, отличная цена/качество";
    if (key == "deepseek-v4.1-flash")
        return "Улучшенная Flash-версия DeepSeek V4";

    // Gemini
    if (key.contains("gemini-3.1-flash-image"))
        return "Быстрая мультимодальная модель Google для генерации изображений";
    if (key.contains("gemini"))
        return "Мультимодальная модель от Google DeepMind";

    // GLM
    if (key == "glm-5.3")
        return "Мощная модель от Zhipu AI для сложных задач";
    if (key == "glm-5.3-flash")
        return "Эффективная нативная мультимодальная модель от Z.ai, созданная для программирования, обработки документов и автономных агентных сценариев";

    // Grok
    if (key == "grok-4.6")
        return "Это флагманская мультимодальная ИИ-модель от компании xAI, выпущенная 12 августа 2026 года";
    if (key == "grok-4.7")
        return "Новое поколение Grok: длиннее контекст, мощнее рассуждения";

    // Grok Image
    if (key == "grok-imagine-image-quality")
        return "Генерация изображений высокого качества от xAI";

    // Kimi
    if (key == "kimi-2.7")
        return "специализированная модель искусственного интеллекта для программирования и агентных задач";
    if (key == "kimi-k3")
        return "Флагманская мультимодальная языковая модель от китайской компании Moonshot AI, выпущенная в июле 2026 года";

    // Qwen
    if (key == "qwen-3.8-max")
        return "Для решения сложных многоступенчатых и автономных задач";

    if (key.contains("opus"))
        return "Мощная модель для сложных задач";
    if (key.contains("sonnet"))
        return "Баланс скорости и качества";
    if (key.contains("gpt"))
        return "Модель OpenAI";
    if (key.contains("deepseek"))
        return "Модель DeepSeek для кода и математики";
    if (key.contains("grok"))
        return "Модель от xAI";
    if (key.contains("kimi"))
        return "Модель от Moonshot AI";
    if (key.contains("qwen"))
        return "Модель Qwen";
    if (key.contains("glm"))
        return "Модель GLM";
    return rawId;
}

ModelSelectorDialog::ModelSelectorDialog(const QVector<ModelEntry> &models,
                                         const QString &currentId,
                                         QWidget *parent)
    : QDialog(parent), m_currentId(currentId)
{
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setModal(true);
    resize(680, 560);

    auto *outer = new QVBoxLayout(this);
    outer->setContentsMargins(20, 20, 20, 20);

    auto *card = new QFrame(this);
    card->setObjectName("card");
    card->setStyleSheet(R"(
        #card {
            background: rgba(13, 13, 15, 0.96);
            border: 1px solid rgba(255, 255, 255, 0.08);
            border-radius: 20px;
        }
    )");
    outer->addWidget(card);

    auto *shadow = new QGraphicsDropShadowEffect(card);
    shadow->setBlurRadius(60);
    shadow->setColor(QColor(0, 0, 0, 220));
    shadow->setOffset(0, 20);
    card->setGraphicsEffect(shadow);

    auto *root = new QVBoxLayout(card);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    auto *header = new QWidget();
    header->setFixedHeight(58);
    auto *hl = new QHBoxLayout(header);
    hl->setContentsMargins(24, 0, 14, 0);

    auto *title = new QLabel("Выбор модели");
    title->setStyleSheet("color: #FFFFFF; font-size: 16px; font-weight: 600;");
    hl->addWidget(title);
    hl->addStretch();

    auto *closeBtn = new QPushButton("✕");
    closeBtn->setFixedSize(34, 34);
    closeBtn->setCursor(Qt::PointingHandCursor);
    closeBtn->setStyleSheet(R"(
        QPushButton { background: transparent; color: #8A8A90; border: none;
                      border-radius: 8px; font-size: 14px; }
        QPushButton:hover { background: rgba(255,255,255,0.06); color: #FFFFFF; }
    )");
    connect(closeBtn, &QPushButton::clicked, this, &QDialog::reject);
    hl->addWidget(closeBtn);
    root->addWidget(header);

    auto *sep = new QFrame();
    sep->setFixedHeight(1);
    sep->setStyleSheet("background: rgba(255,255,255,0.06);");
    root->addWidget(sep);

    auto *body = new QWidget();
    auto *bl = new QHBoxLayout(body);
    bl->setContentsMargins(0, 0, 0, 0);
    bl->setSpacing(0);

    m_catList = new QListWidget();
    m_catList->setFixedWidth(190);
    m_catList->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_catList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_catList->setStyleSheet(R"(
        QListWidget { background: rgba(0,0,0,0.25); border: none; outline: none;
                      padding: 8px 0; border-right: 1px solid rgba(255,255,255,0.06); }
        QListWidget::item { color: #8A8A90; padding: 0; margin: 1px 0;
                            border-left: 3px solid transparent; }
        QListWidget::item:hover { background: rgba(255,255,255,0.04); }
        QListWidget::item:selected { background: rgba(255,255,255,0.06);
                                     border-left: 3px solid #FFFFFF; }
        QScrollBar:vertical { background: transparent; width: 6px; margin: 4px 2px; }
        QScrollBar::handle:vertical { background: rgba(255,255,255,0.10);
                                      border-radius: 3px; min-height: 20px; }
        QScrollBar::add-line, QScrollBar::sub-line { height: 0; }
        QScrollBar::add-page, QScrollBar::sub-page { background: transparent; }
    )");

    m_modelList = new QListWidget();
    m_modelList->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    m_modelList->setStyleSheet(R"(
        QListWidget { background: transparent; border: none; outline: none;
                      padding: 8px 12px; }
        QListWidget::item { background: transparent; border: none; margin: 2px 0; }
        QListWidget::item:selected { background: transparent; }
        QScrollBar:vertical { background: transparent; width: 6px; margin: 4px 2px; }
        QScrollBar::handle:vertical { background: rgba(255,255,255,0.10);
                                      border-radius: 3px; min-height: 20px; }
        QScrollBar::add-line, QScrollBar::sub-line { height: 0; }
        QScrollBar::add-page, QScrollBar::sub-page { background: transparent; }
    )");

    bl->addWidget(m_catList);
    bl->addWidget(m_modelList, 1);
    root->addWidget(body, 1);

    for (const auto &m : models)
    {
        QString fam = m.family;
        if (fam.isEmpty() || fam == "Другие")
            continue;
        if (!m_families.contains(fam))
            m_families.append(fam);
        m_byFamily[fam].append(m);
    }

    QStringList preferred =
        {
        "Opus", "Sonnet", "Haiku", "Fable",
        "GPT", "Gemini",
        "DeepSeek", "GLM", "Grok", "Kimi", "Qwen"
    };

    QStringList sorted;
    for (const auto &f : preferred)
        if (m_families.contains(f))
            sorted.append(f);
    for (const auto &f : m_families)
        if (!sorted.contains(f))
            sorted.append(f);
    m_families = sorted;

    for (const auto &f : m_families)
    {
        auto *item = new QListWidgetItem(m_catList);
        auto *w = new QWidget();
        auto *h = new QHBoxLayout(w);
        h->setContentsMargins(18, 0, 18, 0);
        h->setSpacing(12);

        auto *iconLbl = new QLabel();
        iconLbl->setFixedSize(18, 18);
        iconLbl->setAlignment(Qt::AlignCenter);
        iconLbl->setPixmap(renderSvg(svgForFamily(f), 18, QColor("#8A8A90")));
        iconLbl->setStyleSheet("background: transparent;");

        auto *textLbl = new QLabel(f);
        textLbl->setStyleSheet(
            "color: #8A8A90; font-size: 13px; background: transparent;");
        textLbl->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);

        h->addWidget(iconLbl, 0, Qt::AlignVCenter);
        h->addWidget(textLbl, 1, Qt::AlignVCenter);

        item->setSizeHint(QSize(0, 40));
        m_catList->setItemWidget(item, w);
        item->setData(Qt::UserRole, f);
    }

    auto updateCatStyles = [this]()
    {
        for (int i = 0; i < m_catList->count(); ++i)
        {
            auto *it = m_catList->item(i);
            auto *w = m_catList->itemWidget(it);
            if (!w)
                continue;

            auto *iconLbl = w->findChild<QLabel*>();
            if (!iconLbl)
                continue;

            QString fam = it->data(Qt::UserRole).toString();
            bool sel = it->isSelected();
            iconLbl->setPixmap(renderSvg(svgForFamily(fam), 18,
                                         sel ? QColor("#FFFFFF") : QColor("#8A8A90")));
        }
    };

    auto *bottom = new QWidget();
    bottom->setFixedHeight(62);
    auto *botL = new QHBoxLayout(bottom);
    botL->setContentsMargins(24, 10, 24, 14);

    auto *selectBtn = new QPushButton("Выбрать");
    selectBtn->setFixedHeight(38);
    selectBtn->setCursor(Qt::PointingHandCursor);
    selectBtn->setStyleSheet(R"(
        QPushButton { background: #FFFFFF; color: #0A0A0A; border: none;
                      border-radius: 10px; padding: 0 24px;
                      font-weight: 600; font-size: 13px; }
        QPushButton:hover { background: #E8E8EA; }
    )");
    connect(selectBtn, &QPushButton::clicked, this, [this](){
        auto *it = m_modelList->currentItem();
        if (!it) return;
        selectedModel = it->data(Qt::UserRole).toString();
        selectedName  = it->data(Qt::UserRole + 1).toString();
        accept();
    });
    botL->addStretch();
    botL->addWidget(selectBtn);
    root->addWidget(bottom);

    connect(m_catList, &QListWidget::currentRowChanged,
            this, [this, updateCatStyles](int row){
                if (row < 0 || row >= m_families.size()) return;
                populate(m_families[row]);
                if (m_modelList->count() > 0) m_modelList->setCurrentRow(0);
                refreshStyle();
                updateCatStyles();
            });

    connect(m_catList, &QListWidget::itemSelectionChanged,
            this, updateCatStyles);

    connect(m_modelList, &QListWidget::itemSelectionChanged,
            this, &ModelSelectorDialog::refreshStyle);

    connect(m_modelList, &QListWidget::itemClicked,
            this, [this](QListWidgetItem *it){
                selectedModel = it->data(Qt::UserRole).toString();
                selectedName  = it->data(Qt::UserRole + 1).toString();
                accept();
            });

    int initialCat = 0;
    for (int i = 0; i < m_families.size(); ++i)
        for (const auto &m : m_byFamily.value(m_families[i]))
            if (m.id == m_currentId)
            {
                initialCat = i;
                break;
            }

    populate(m_families.value(initialCat));
    if (m_modelList->count() > 0)
        m_modelList->setCurrentRow(0);
    refreshStyle();

    m_catList->blockSignals(true);
    m_catList->setCurrentRow(initialCat);
    m_catList->blockSignals(false);
    updateCatStyles();

    auto *hint = new QWidget(m_modelList->viewport());
    hint->setObjectName("scrollHint");
    hint->setStyleSheet(R"(
        #scrollHint {
            background: qlineargradient(x1:0, y1:1, x2:0, y2:0,
                                        stop:0 rgba(13,13,15,230),
                                        stop:1 rgba(13,13,15,0));
        }
    )");
    hint->setFixedHeight(40);
    hint->setAttribute(Qt::WA_TransparentForMouseEvents, true);

    auto updateHint = [this, hint]() {
        auto *sb = m_modelList->verticalScrollBar();
        bool more = sb && (sb->maximum() > sb->value());
        hint->setVisible(more);
        if (hint)
        {
            hint->setGeometry(0,
                              m_modelList->viewport()->height() - hint->height(),
                              m_modelList->viewport()->width(),
                              hint->height());
            hint->raise();
        }
    };

    connect(m_modelList->verticalScrollBar(), &QScrollBar::valueChanged,
            this, [updateHint](int){ updateHint(); });
    connect(m_modelList->verticalScrollBar(), &QScrollBar::rangeChanged,
            this, [updateHint](int,int){ updateHint(); });
    connect(m_catList, &QListWidget::currentRowChanged,
            this, [updateHint](int){ QTimer::singleShot(50, updateHint); });

    QTimer::singleShot(100, updateHint);
}

void ModelSelectorDialog::populate(const QString &family)
{
    if (!m_modelList)
        return;

    m_modelList->clear();
    int listWidth = m_modelList->viewport()->width();
    int cardWidth = qMax(320, int(listWidth * 0.70));

    const auto &items = m_byFamily.value(family);
    for (const auto &m : items)
    {
        auto *item = new QListWidgetItem(m_modelList);
        auto *w = new QWidget();
        auto *wl = new QVBoxLayout(w);
        wl->setContentsMargins(14, 12, 14, 12);
        wl->setSpacing(6);

        auto *row = new QHBoxLayout();
        row->setSpacing(8);

        auto *nameLbl = new QLabel(m.display.isEmpty() ? m.id : m.display);
        nameLbl->setStyleSheet(
            "color: #FFFFFF; font-size: 14px; font-weight: 600;"
            "background: transparent;");
        row->addWidget(nameLbl);
        row->addStretch();

        if (m.id == m_currentId)
        {
            auto *check = new QLabel("✓");
            check->setStyleSheet(
                "color: #FFFFFF; font-size: 14px; background: transparent;");
            row->addWidget(check, 0, Qt::AlignTop);
        }
        wl->addLayout(row);

        QString desc = descriptionForId(m.id);
        auto *descLbl = new QLabel(desc);
        descLbl->setWordWrap(true);
        descLbl->setStyleSheet(
            "color: #75757C; font-size: 12px; background: transparent;");
        wl->addWidget(descLbl);

        wl->addStretch();

        w->setStyleSheet("background: transparent; border: none;");
        w->setFixedWidth(cardWidth);

        w->adjustSize();
        int h = w->sizeHint().height() + 8;
        h = qMax(60, h);

        item->setSizeHint(QSize(0, h));
        m_modelList->setItemWidget(item, w);
        item->setData(Qt::UserRole, m.id);
        item->setData(Qt::UserRole + 1, m.display.isEmpty() ? m.id : m.display);
    }

    QTimer::singleShot(0, this, [this]()
    {
        int lw = m_modelList->viewport()->width();
        int cw = qMax(320, int(lw * 0.70));

        for (int i = 0; i < m_modelList->count(); ++i)
        {
            auto *it = m_modelList->item(i);
            auto *w = m_modelList->itemWidget(it);
            if (!w)
                continue;

            w->setFixedWidth(cw);
            w->adjustSize();
            int h = qMax(60, w->sizeHint().height() + 8);
            it->setSizeHint(QSize(0, h));
        }
    });
}

void ModelSelectorDialog::refreshStyle()
{
    if (!m_modelList)
        return;
    for (int i = 0; i < m_modelList->count(); ++i)
    {
        auto *it = m_modelList->item(i);
        auto *w = m_modelList->itemWidget(it);
        if (!w)
            continue;
        w->setStyleSheet("background: transparent; border: none;");
    }
}
