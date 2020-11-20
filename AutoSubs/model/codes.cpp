#include "codes.h"

const std::map<int, char32_t> id_to_char = {
    {0, U'_'},
    {1, U'^'},
    {2, U'$'},
    {3, U' '},
    {4, U'그'},
    {5, U'이'},
    {6, U'는'},
    {7, U'아'},
    {8, U'가'},
    {9, U'고'},
    {10, U'어'},
    {11, U'거'},
    {12, U'지'},
    {13, U'데'},
    {14, U'?'},
    {15, U'나'},
    {16, U'하'},
    {17, U'다'},
    {18, U'서'},
    {19, U'에'},
    {20, U'도'},
    {21, U'게'},
    {22, U'니'},
    {23, U'기'},
    {24, U'은'},
    {25, U'면'},
    {26, U'야'},
    {27, U'있'},
    {28, U'한'},
    {29, U'을'},
    {30, U'까'},
    {31, U'해'},
    {32, U'리'},
    {33, U'라'},
    {34, U'래'},
    {35, U'사'},
    {36, U'근'},
    {37, U'들'},
    {38, U'안'},
    {39, U'로'},
    {40, U'일'},
    {41, U'뭐'},
    {42, U'내'},
    {43, U'보'},
    {44, U'제'},
    {45, U'같'},
    {46, U'자'},
    {47, U'만'},
    {48, U'시'},
    {49, U'런'},
    {50, U'너'},
    {51, U'대'},
    {52, U'때'},
    {53, U'되'},
    {54, U'으'},
    {55, U'진'},
    {56, U'를'},
    {57, U'잖'},
    {58, U'오'},
    {59, U'러'},
    {60, U'인'},
    {61, U'막'},
    {62, U'무'},
    {63, U'었'},
    {64, U'구'},
    {65, U'했'},
    {66, U'수'},
    {67, U'간'},
    {68, U'애'},
    {69, U'우'},
    {70, U'요'},
    {71, U'마'},
    {72, U'생'},
    {73, U'렇'},
    {74, U'냥'},
    {75, U'짜'},
    {76, U'주'},
    {77, U'없'},
    {78, U'말'},
    {79, U'학'},
    {80, U'스'},
    {81, U'더'},
    {82, U'많'},
    {83, U'원'},
    {84, U'음'},
    {85, U'정'},
    {86, U'겠'},
    {87, U'여'},
    {88, U'먹'},
    {89, U'금'},
    {90, U'든'},
    {91, U'부'},
    {92, U'할'},
    {93, U'전'},
    {94, U'번'},
    {95, U'좋'},
    {96, U'랑'},
    {97, U'네'},
    {98, U'람'},
    {99, U'약'},
    {100, U'건'},
    {101, U'각'},
    {102, U'좀'},
    {103, U'알'},
    {104, U'잘'},
    {105, U'걸'},
    {106, U'모'},
    {107, U'것'},
    {108, U'상'},
    {109, U'려'},
    {110, U'장'},
    {111, U'히'},
    {112, U'않'},
    {113, U'맞'},
    {114, U'던'},
    {115, U'르'},
    {116, U'교'},
    {117, U'바'},
    {118, U'냐'},
    {119, U'드'},
    {120, U'십'},
    {121, U'날'},
    {122, U'치'},
    {123, U'비'},
    {124, U'단'},
    {125, U'동'},
    {126, U'또'},
    {127, U'못'},
    {128, U'저'},
    {129, U'얘'},
    {130, U'중'},
    {131, U'의'},
    {132, U'난'},
    {133, U'엄'},
    {134, U'봤'},
    {135, U'걔'},
    {136, U'화'},
    {137, U'응'},
    {138, U'싶'},
    {139, U'갔'},
    {140, U'았'},
    {141, U'집'},
    {142, U'왜'},
    {143, U'계'},
    {144, U'공'},
    {145, U'긴'},
    {146, U'신'},
    {147, U'적'},
    {148, U'연'},
    {149, U'직'},
    {150, U'실'},
    {151, U'영'},
    {152, U'미'},
    {153, U'봐'},
    {154, U'분'},
    {155, U'테'},
    {156, U'년'},
    {157, U'트'},
    {158, U'문'},
    {159, U'와'},
    {160, U'돼'},
    {161, U'물'},
    {162, U'예'},
    {163, U'터'},
    {164, U'세'},
    {165, U'럼'},
    {166, U'청'},
    {167, U'차'},
    {168, U'친'},
    {169, U'개'},
    {170, U'삼'},
    {171, U'국'},
    {172, U'두'},
    {173, U'소'},
    {174, U'살'},
    {175, U'재'},
    {176, U'운'},
    {177, U'쫌'},
    {178, U'유'},
    {179, U'속'},
    {180, U'명'},
    {181, U'랬'},
    {182, U'본'},
    {183, U'갈'},
    {184, U'방'},
    {185, U'돈'},
    {186, U'타'},
    {187, U'처'},
    {188, U'빠'},
    {189, U'과'},
    {190, U'식'},
    {191, U'디'},
    {192, U'배'},
    {193, U'피'},
    {194, U'뭔'},
    {195, U'선'},
    {196, U'남'},
    {197, U'경'},
    {198, U'달'},
    {199, U'언'},
    {200, U'받'},
    {201, U'심'},
    {202, U'월'},
    {203, U'버'},
    {204, U'왔'},
    {205, U'느'},
    {206, U'점'},
    {207, U'올'},
    {208, U'업'},
    {209, U'른'},
    {210, U'성'},
    {211, U'회'},
    {212, U'조'},
    {213, U'워'},
    {214, U'따'},
    {215, U'행'},
    {216, U'반'},
    {217, U'님'},
    {218, U'딱'},
    {219, U'관'},
    {220, U'입'},
    {221, U'카'},
    {222, U'당'},
    {223, U'였'},
    {224, U'케'},
    {225, U'쪽'},
    {226, U'천'},
    {227, U'작'},
    {228, U'누'},
    {229, U'열'},
    {230, U'얼'},
    {231, U'울'},
    {232, U'찮'},
    {233, U'된'},
    {234, U'별'},
    {235, U'떻'},
    {236, U'머'},
    {237, U'쓰'},
    {238, U'위'},
    {239, U'크'},
    {240, U'노'},
    {241, U'괜'},
    {242, U'강'},
    {243, U'편'},
    {244, U'몰'},
    {245, U'맛'},
    {246, U'준'},
    {247, U'줄'},
    {248, U'파'},
    {249, U'백'},
    {250, U'매'},
    {251, U'산'},
    {252, U'술'},
    {253, U'힘'},
    {254, U'프'},
    {255, U'즘'},
    {256, U'임'},
    {257, U'체'},
    {258, U'형'},
    {259, U'몇'},
    {260, U'맨'},
    {261, U'새'},
    {262, U'용'},
    {263, U'키'},
    {264, U'통'},
    {265, U'양'},
    {266, U'끝'},
    {267, U'싸'},
    {268, U'볼'},
    {269, U'혼'},
    {270, U'온'},
    {271, U'등'},
    {272, U'길'},
    {273, U'될'},
    {274, U'밌'},
    {275, U'육'},
    {276, U'늘'},
    {277, U'슨'},
    {278, U'됐'},
    {279, U'놀'},
    {280, U'외'},
    {281, U'팔'},
    {282, U'져'},
    {283, U'레'},
    {284, U'억'},
    {285, U'발'},
    {286, U'결'},
    {287, U'초'},
    {288, U'감'},
    {289, U'군'},
    {290, U'호'},
    {291, U'름'},
    {292, U'솔'},
    {293, U'닌'},
    {294, U'밖'},
    {295, U'불'},
    {296, U'밥'},
    {297, U'포'},
    {298, U'싫'},
    {299, U'완'},
    {300, U'갖'},
    {301, U'겨'},
    {302, U'질'},
    {303, U'토'},
    {304, U'험'},
    {305, U'색'},
    {306, U'떤'},
    {307, U'역'},
    {308, U'티'},
    {309, U'갑'},
    {310, U'목'},
    {311, U'린'},
    {312, U'추'},
    {313, U'격'},
    {314, U'후'},
    {315, U'확'},
    {316, U'루'},
    {317, U'민'},
    {318, U'끼'},
    {319, U'칠'},
    {320, U'돌'},
    {321, U'찍'},
    {322, U'쪼'},
    {323, U'깐'},
    {324, U'필'},
    {325, U'빨'},
    {326, U'났'},
    {327, U'락'},
    {328, U'박'},
    {329, U'끔'},
    {330, U'낌'},
    {331, U'럴'},
    {332, U'취'},
    {333, U'복'},
    {334, U'둘'},
    {335, U'페'},
    {336, U'렸'},
    {337, U'써'},
    {338, U'줘'},
    {339, U'급'},
    {340, U'력'},
    {341, U'잡'},
    {342, U'씩'},
    {343, U'찾'},
    {344, U'놓'},
    {345, U'최'},
    {346, U'코'},
    {347, U'넘'},
    {348, U'졌'},
    {349, U'섯'},
    {350, U'브'},
    {351, U'현'},
    {352, U'눈'},
    {353, U'항'},
    {354, U'귀'},
    {355, U'설'},
    {356, U'벌'},
    {357, U'담'},
    {358, U'앞'},
    {359, U'책'},
    {360, U'절'},
    {361, U'플'},
    {362, U'폰'},
    {363, U'태'},
    {364, U'종'},
    {365, U'옛'},
    {366, U'증'},
    {367, U'튼'},
    {368, U'글'},
    {369, U'습'},
    {370, U'병'},
    {371, U'론'},
    {372, U'출'},
    {373, U'능'},
    {374, U'침'},
    {375, U'순'},
    {376, U'줬'},
    {377, U'평'},
    {378, U'메'},
    {379, U'똑'},
    {380, U'커'},
    {381, U'엔'},
    {382, U'꾸'},
    {383, U'란'},
    {384, U'듣'},
    {385, U'씨'},
    {386, U'큰'},
    {387, U'표'},
    {388, U'잠'},
    {389, U'먼'},
    {390, U'쁘'},
    {391, U'활'},
    {392, U'합'},
    {393, U'접'},
    {394, U'럽'},
    {395, U'옷'},
    {396, U'쳐'},
    {397, U'손'},
    {398, U'붙'},
    {399, U'망'},
    {400, U'죽'},
    {401, U'투'},
    {402, U'족'},
    {403, U'셨'},
    {404, U'참'},
    {405, U'떨'},
    {406, U'웃'},
    {407, U'졸'},
    {408, U'쉬'},
    {409, U'뭘'},
    {410, U'변'},
    {411, U'릴'},
    {412, U'웠'},
    {413, U'홍'},
    {414, U'즈'},
    {415, U'랐'},
    {416, U'독'},
    {417, U'충'},
    {418, U'짝'},
    {419, U'떡'},
    {420, U'뒤'},
    {421, U'휴'},
    {422, U'셔'},
    {423, U'넣'},
    {424, U'쨌'},
    {425, U'악'},
    {426, U'패'},
    {427, U'빼'},
    {428, U'슬'},
    {429, U'특'},
    {430, U'꺼'},
    {431, U'숙'},
    {432, U'쯤'},
    {433, U'텐'},
    {434, U'창'},
    {435, U'겼'},
    {436, U'굴'},
    {437, U'판'},
    {438, U'죠'},
    {439, U'답'},
    {440, U'희'},
    {441, U'허'},
    {442, U'옆'},
    {443, U'료'},
    {444, U'닐'},
    {445, U'택'},
    {446, U'림'},
    {447, U'읽'},
    {448, U'핸'},
    {449, U'축'},
    {450, U'풀'},
    {451, U'틀'},
    {452, U'몸'},
    {453, U'골'},
    {454, U'황'},
    {455, U'켜'},
    {456, U'익'},
    {457, U'베'},
    {458, U'북'},
    {459, U'법'},
    {460, U'늦'},
    {461, U'함'},
    {462, U'랜'},
    {463, U'꼬'},
    {464, U'향'},
    {465, U'석'},
    {466, U'환'},
    {467, U'슷'},
    {468, U'품'},
    {469, U'혀'},
    {470, U'블'},
    {471, U'쓸'},
    {472, U'채'},
    {473, U'며'},
    {474, U'욕'},
    {475, U'권'},
    {476, U'검'},
    {477, U'굳'},
    {478, U'록'},
    {479, U'톡'},
    {480, U'김'},
    {481, U'넌'},
    {482, U'깨'},
    {483, U'션'},
    {484, U'캐'},
    {485, U'송'},
    {486, U'녀'},
    {487, U'탈'},
    {488, U'광'},
    {489, U'혹'},
    {490, U'퍼'},
    {491, U'뽑'},
    {492, U'철'},
    {493, U'째'},
    {494, U'움'},
    {495, U'밤'},
    {496, U'꼭'},
    {497, U'샀'},
    {498, U'끊'},
    {499, U'땐'},
    {500, U'깔'},
    {501, U'멀'},
    {502, U'높'},
    {503, U'께'},
    {504, U'큼'},
    {505, U'녁'},
    {506, U'곳'},
    {507, U'잔'},
    {508, U'쉽'},
    {509, U'짐'},
    {510, U'암'},
    {511, U'극'},
    {512, U'련'},
    {513, U'떠'},
    {514, U'벽'},
    {515, U'헤'},
    {516, U'C'},
    {517, U'끄'},
    {518, U'곱'},
    {519, U'승'},
    {520, U'봉'},
    {521, U'착'},
    {522, U'촌'},
    {523, U'껴'},
    {524, U'딩'},
    {525, U'류'},
    {526, U'뜨'},
    {527, U'넷'},
    {528, U'놨'},
    {529, U'궁'},
    {530, U'논'},
    {531, U'곤'},
    {532, U'클'},
    {533, U'싼'},
    {534, U'앉'},
    {535, U'컴'},
    {536, U'맥'},
    {537, U'팀'},
    {538, U'썼'},
    {539, U'낫'},
    {540, U'튜'},
    {541, U'걱'},
    {542, U'쁜'},
    {543, U'킨'},
    {544, U'빌'},
    {545, U'쿠'},
    {546, U'찌'},
    {547, U'쌤'},
    {548, U'T'},
    {549, U'밀'},
    {550, U'빵'},
    {551, U'냈'},
    {552, U'센'},
    {553, U'딴'},
    {554, U'쩌'},
    {555, U'딸'},
    {556, U'걍'},
    {557, U'획'},
    {558, U'씬'},
    {559, U'챙'},
    {560, U'첫'},
    {561, U'범'},
    {562, U'핑'},
    {563, U'굉'},
    {564, U'쩔'},
    {565, U'팅'},
    {566, U'긍'},
    {567, U'탄'},
    {568, U'덟'},
    {569, U'퇴'},
    {570, U'뛰'},
    {571, U'층'},
    {572, U'춰'},
    {573, U'훨'},
    {574, U'찬'},
    {575, U'듯'},
    {576, U'S'},
    {577, U'왕'},
    {578, U'텔'},
    {579, U'뉴'},
    {580, U'렌'},
    {581, U'탕'},
    {582, U'짓'},
    {583, U'밑'},
    {584, U'헬'},
    {585, U'존'},
    {586, U'립'},
    {587, U'녔'},
    {588, U'꼈'},
    {589, U'빡'},
    {590, U'낮'},
    {591, U'견'},
    {592, U'링'},
    {593, U'볶'},
    {594, U'낙'},
    {595, U'릭'},
    {596, U'젠'},
    {597, U'퓨'},
    {598, U'츠'},
    {599, U'맘'},
    {600, U'놔'},
    {601, U'렵'},
    {602, U'땜'},
    {603, U'쇼'},
    {604, U'값'},
    {605, U'닭'},
    {606, U'깝'},
    {607, U'픈'},
    {608, U'탁'},
    {609, U'쓴'},
    {610, U'농'},
    {611, U'량'},
    {612, U'염'},
    {613, U'홉'},
    {614, U'척'},
    {615, U'겁'},
    {616, U'콘'},
    {617, U'섭'},
    {618, U'냄'},
    {619, U'P'},
    {620, U'효'},
    {621, U'규'},
    {622, U'꿈'},
    {623, U'곡'},
    {624, U'액'},
    {625, U'쎄'},
    {626, U'덜'},
    {627, U'턴'},
    {628, U'킹'},
    {629, U'훈'},
    {630, U'쳤'},
    {631, U'널'},
    {632, U'멋'},
    {633, U'꿀'},
    {634, U'깜'},
    {635, U'짧'},
    {636, U'롤'},
    {637, U'낼'},
    {638, U'꽤'},
    {639, U'총'},
    {640, U'램'},
    {641, U'덕'},
    {642, U'믄'},
    {643, U'믿'},
    {644, U'흥'},
    {645, U'롱'},
    {646, U'뜻'},
    {647, U'짤'},
    {648, U'쌍'},
    {649, U'컨'},
    {650, U'셋'},
    {651, U'잤'},
    {652, U'닥'},
    {653, U'웬'},
    {654, U'엽'},
    {655, U'혜'},
    {656, U'찰'},
    {657, U'뻐'},
    {658, U'뿌'},
    {659, U'빈'},
    {660, U'꿔'},
    {661, U'낸'},
    {662, U'뻔'},
    {663, U'쌓'},
    {664, U'즐'},
    {665, U'튀'},
    {666, U'겹'},
    {667, U'득'},
    {668, U'끌'},
    {669, U'M'},
    {670, U'V'},
    {671, U'녹'},
    {672, U'푸'},
    {673, U'쭉'},
    {674, U'싱'},
    {675, U'팬'},
    {676, U'A'},
    {677, U'!'},
    {678, U'념'},
    {679, U'맡'},
    {680, U'쟁'},
    {681, U'엑'},
    {682, U'켓'},
    {683, U'뀌'},
    {684, U'털'},
    {685, U'풍'},
    {686, U'웨'},
    {687, U'땡'},
    {688, U'롯'},
    {689, U'롭'},
    {690, U'젊'},
    {691, U'넓'},
    {692, U'멘'},
    {693, U'냉'},
    {694, U'칼'},
    {695, U'잉'},
    {696, U'빙'},
    {697, U'뿐'},
    {698, U'옮'},
    {699, U'젤'},
    {700, U'B'},
    {701, U'죄'},
    {702, U'탔'},
    {703, U'샤'},
    {704, U'홀'},
    {705, U'떼'},
    {706, U'줌'},
    {707, U'징'},
    {708, U'폭'},
    {709, U'G'},
    {710, U'킬'},
    {711, U'흔'},
    {712, U'딜'},
    {713, U'슈'},
    {714, U'율'},
    {715, U'즌'},
    {716, U'씀'},
    {717, U'앙'},
    {718, U'눠'},
    {719, U'콩'},
    {720, U'얻'},
    {721, U'숨'},
    {722, U'닝'},
    {723, U'꽃'},
    {724, U'쌀'},
    {725, U'컬'},
    {726, U'춤'},
    {727, U'c'},
    {728, U'뚫'},
    {729, U'엠'},
    {730, U'몬'},
    {731, U'D'},
    {732, U'흐'},
    {733, U'앤'},
    {734, U'똥'},
    {735, U'콜'},
    {736, U'델'},
    {737, U'렀'},
    {738, U'폐'},
    {739, U'엘'},
    {740, U'쁠'},
    {741, U'랄'},
    {742, U'걘'},
    {743, U'벤'},
    {744, U'봄'},
    {745, U'왠'},
    {746, U'씻'},
    {747, U'률'},
    {748, U'켰'},
    {749, U'짱'},
    {750, U'웹'},
    {751, U'압'},
    {752, U'럭'},
    {753, U'땅'},
    {754, U'멍'},
    {755, U'랩'},
    {756, U'댓'},
    {757, U'깊'},
    {758, U'뮤'},
    {759, U'령'},
    {760, U'릿'},
    {761, U'낀'},
    {762, U'윤'},
    {763, U'옥'},
    {764, U'룸'},
    {765, U'딘'},
    {766, U'객'},
    {767, U'댄'},
    {768, U'컵'},
    {769, U'폴'},
    {770, U'쟤'},
    {771, U'뷰'},
    {772, U'템'},
    {773, U'덴'},
    {774, U'눌'},
    {775, U'캠'},
    {776, U'홈'},
    {777, U'삶'},
    {778, U'삭'},
    {779, U'벨'},
    {780, U'엉'},
    {781, U'헐'},
    {782, U'벅'},
    {783, U'벗'},
    {784, U'혈'},
    {785, U'밍'},
    {786, U'셀'},
    {787, U'낭'},
    {788, U'춥'},
    {789, U'릉'},
    {790, U't'},
    {791, U'잃'},
    {792, U'I'},
    {793, U'놈'},
    {794, U'춘'},
    {795, U'찜'},
    {796, U'R'},
    {797, U'걷'},
    {798, U'삐'},
    {799, U'헌'},
    {800, U'딨'},
    {801, U'빛'},
    {802, U'흘'},
    {803, U'닫'},
    {804, U'균'},
    {805, U'p'},
    {806, U'L'},
    {807, U'좌'},
    {808, U'껄'},
    {809, U'펜'},
    {810, U'N'},
    {811, U'싹'},
    {812, U'탑'},
    {813, U'쏘'},
    {814, U'O'},
    {815, U'픽'},
    {816, U'덩'},
    {817, U'햄'},
    {818, U'큐'},
    {819, U'힐'},
    {820, U'곧'},
    {821, U'낳'},
    {822, U'힌'},
    {823, U'팩'},
    {824, U'뒷'},
    {825, U'툰'},
    {826, U'섬'},
    {827, U'꽂'},
    {828, U'례'},
    {829, U'핫'},
    {830, U'섞'},
    {831, U'촬'},
    {832, U'흰'},
    {833, U'둥'},
    {834, U'K'},
    {835, U'괴'},
    {836, U's'},
    {837, U'핀'},
    {838, U'꿨'},
    {839, U'틱'},
    {840, U'밝'},
    {841, U'랙'},
    {842, U'땠'},
    {843, U'둔'},
    {844, U'슴'},
    {845, U'첨'},
    {846, U'밴'},
    {847, U'렁'},
    {848, U'칭'},
    {849, U'묻'},
    {850, U'뜬'},
    {851, U'깎'},
    {852, U'엇'},
    {853, U'컸'},
    {854, U'퀴'},
    {855, U'납'},
    {856, U'협'},
    {857, U'몽'},
    {858, U'꼐'},
    {859, U'떴'},
    {860, U'썰'},
    {861, U'찐'},
    {862, U'꼴'},
    {863, U'갠'},
    {864, U'턱'},
    {865, U'틴'},
    {866, U'낄'},
    {867, U'뒀'},
    {868, U'끗'},
    {869, U'꼼'},
    {870, U'F'},
    {871, U'샵'},
    {872, U'휘'},
    {873, U'뼈'},
    {874, U'뚜'},
    {875, U'쩍'},
    {876, U'팡'},
    {877, U'멜'},
    {878, U'톤'},
    {879, U'앨'},
    {880, U'탐'},
    {881, U'칸'},
    {882, U'끓'},
    {883, U'뚱'},
    {884, U'닮'},
    {885, U'깃'},
    {886, U'짬'},
    {887, U'빤'},
    {888, U'측'},
    {889, U'혔'},
    {890, U'꽁'},
    {891, U'펴'},
    {892, U'앴'},
    {893, U'겸'},
    {894, U'쿨'},
    {895, U'릇'},
    {896, U'얀'},
    {897, U'쿄'},
    {898, U'컷'},
    {899, U'팠'},
    {900, U'끈'},
    {901, U'렴'},
    {902, U'잊'},
    {903, U'덤'},
    {904, U'갤'},
    {905, U'븐'},
    {906, U'흡'},
    {907, U'덮'},
    {908, U'씹'},
    {909, U'뽀'},
    {910, U'뚝'},
    {911, U'갚'},
    {912, U'찔'},
    {913, U'댔'},
    {914, U'혁'},
    {915, U'띠'},
    {916, U'벼'},
    {917, U'얇'},
    {918, U'뺐'},
    {919, U'팝'},
    {920, U'잇'},
    {921, U'왼'},
    {922, U'낚'},
    {923, U'칙'},
    {924, U'겉'},
    {925, U'뜯'},
    {926, U'닦'},
    {927, U'짠'},
    {928, U'썹'},
    {929, U'뷔'},
    {930, U'묶'},
    {931, U'꾼'},
    {932, U'빅'},
    {933, U'땄'},
    {934, U'캡'},
    {935, U'묘'},
    {936, U'샘'},
    {937, U'묵'},
    {938, U'a'},
    {939, U'쭈'},
    {940, U'b'},
    {941, U'겪'},
    {942, U'둬'},
    {943, U'J'},
    {944, U'쫄'},
    {945, U'랫'},
    {946, U'뀐'},
    {947, U'흑'},
    {948, U'댕'},
    {949, U'꽉'},
    {950, U'곰'},
    {951, U'붕'},
    {952, U'땀'},
    {953, U'릎'},
    {954, U'뽕'},
    {955, U'쥐'},
    {956, U'렉'},
    {957, U'숭'},
    {958, U'샐'},
    {959, U'v'},
    {960, U'렛'},
    {961, U'녕'},
    {962, U'힙'},
    {963, U'쫙'},
    {964, U'촉'},
    {965, U'쩜'},
    {966, U'긋'},
    {967, U'샌'},
    {968, U'o'},
    {969, U'쫓'},
    {970, U'쩐'},
    {971, U'헷'},
    {972, U'X'},
    {973, U'웅'},
    {974, U'뺏'},
    {975, U'쵸'},
    {976, U'쪘'},
    {977, U'랍'},
    {978, U'E'},
    {979, U'좁'},
    {980, U'앱'},
    {981, U'썸'},
    {982, U'냅'},
    {983, U'펙'},
    {984, U'늙'},
    {985, U'껌'},
    {986, U'n'},
    {987, U'e'},
    {988, U'랭'},
    {989, U'귤'},
    {990, U'찢'},
    {991, U'닿'},
    {992, U'띄'},
    {993, U'긁'},
    {994, U'귄'},
    {995, U'굽'},
    {996, U'갓'},
    {997, U'캔'},
    {998, U'멈'},
    {999, U'욱'},
    {1000, U'뺄'},
    {1001, U'뇌'},
    {1002, U'팟'},
    {1003, U'쌌'},
    {1004, U'룹'},
    {1005, U'덥'},
    {1006, U'폼'},
    {1007, U'톱'},
    {1008, U'듬'},
    {1009, U'껍'},
    {1010, U'흠'},
    {1011, U'팍'},
    {1012, U'맹'},
    {1013, U'쉴'},
    {1014, U'썩'},
    {1015, U'밟'},
    {1016, U'맵'},
    {1017, U'돋'},
    {1018, U'콤'},
    {1019, U'맙'},
    {1020, U'뱅'},
    {1021, U'쫍'},
    {1022, U'윗'},
    {1023, U'뜩'},
    {1024, U'찝'},
    {1025, U'뺀'},
    {1026, U'닷'},
    {1027, U'넨'},
    {1028, U'쌈'},
    {1029, U'쩨'},
    {1030, U'붓'},
    {1031, U'쩡'},
    {1032, U'믹'},
    {1033, U'잼'},
    {1034, U'r'},
    {1035, U'쭐'},
    {1036, U'엊'},
    {1037, U'g'},
    {1038, U'췄'},
    {1039, U'룩'},
    {1040, U'텀'},
    {1041, U'쇠'},
    {1042, U'숫'},
    {1043, U'풋'},
    {1044, U'쌩'},
    {1045, U'쾌'},
    {1046, U'볍'},
    {1047, U'뤄'},
    {1048, U'겐'},
    {1049, U'm'},
    {1050, U'펌'},
    {1051, U'쪄'},
    {1052, U'뻥'},
    {1053, U'i'},
    {1054, U'뻤'},
    {1055, U'k'},
    {1056, U'핵'},
    {1057, U'셉'},
    {1058, U'듀'},
    {1059, U'닉'},
    {1060, U'략'},
    {1061, U'넉'},
    {1062, U'딪'},
    {1063, U'낯'},
    {1064, U'텍'},
    {1065, U'뱃'},
    {1066, U'멤'},
    {1067, U'윈'},
    {1068, U'엎'},
    {1069, U'뭉'},
    {1070, U'젝'},
    {1071, U'셜'},
    {1072, U'빴'},
    {1073, U'룰'},
    {1074, U'앗'},
    {1075, U'궈'},
    {1076, U'윙'},
    {1077, U'엥'},
    {1078, U'd'},
    {1079, U'꼽'},
    {1080, U'챔'},
    {1081, U'쉐'},
    {1082, U'봇'},
    {1083, U'푼'},
    {1084, U'댁'},
    {1085, U'칵'},
    {1086, U'뿔'},
    {1087, U'뺑'},
    {1088, U'탱'},
    {1089, U'쿼'},
    {1090, U'갱'},
    {1091, U'퉁'},
    {1092, U'빔'},
    {1093, U'썬'},
    {1094, U'빽'},
    {1095, U'둑'},
    {1096, U'헛'},
    {1097, U'빗'},
    {1098, U'탓'},
    {1099, U'륵'},
    {1100, U'꼰'},
    {1101, U'쎈'},
    {1102, U'쥬'},
    {1103, U'깡'},
    {1104, U'퀄'},
    {1105, U'빚'},
    {1106, U'즉'},
    {1107, U'삿'},
    {1108, U'밭'},
    {1109, U'u'},
    {1110, U'혐'},
    {1111, U'햇'},
    {1112, U'툭'},
    {1113, U'탠'},
    {1114, U'샷'},
    {1115, U'맣'},
    {1116, U'껏'},
    {1117, U'핏'},
    {1118, U'앵'},
    {1119, U'뜰'},
    {1120, U'굿'},
    {1121, U'U'},
    {1122, U'섹'},
    {1123, U'펑'},
    {1124, U'맻'},
    {1125, U'뀔'},
    {1126, U'깥'},
    {1127, U'뱀'},
    {1128, U'뢰'},
    {1129, U'껀'},
    {1130, U'뉘'},
    {1131, U'흉'},
    {1132, U'틈'},
    {1133, U'쏟'},
    {1134, U'훔'},
    {1135, U'쇄'},
    {1136, U'뎅'},
    {1137, U'칩'},
    {1138, U'띵'},
    {1139, U'푹'},
    {1140, U'넥'},
    {1141, U'퀘'},
    {1142, U'훅'},
    {1143, U'융'},
    {1144, U'멸'},
    {1145, U'냠'},
    {1146, U'횟'},
    {1147, U'찼'},
    {1148, U'Y'},
    {1149, U'룬'},
    {1150, U'귈'},
    {1151, U'H'},
    {1152, U'젓'},
    {1153, U'쏠'},
    {1154, U'숲'},
    {1155, U'냬'},
    {1156, U'l'},
    {1157, U'짰'},
    {1158, U'멕'},
    {1159, U'뇨'},
    {1160, U'팽'},
    {1161, U'깼'},
    {1162, U'숏'},
    {1163, U'굔'},
    {1164, U'슐'},
    {1165, U'쉰'},
    {1166, U'얄'},
    {1167, U'뱉'},
    {1168, U'렬'},
    {1169, U'굶'},
    {1170, U'팁'},
    {1171, U'츄'},
    {1172, U'뭣'},
    {1173, U'륙'},
    {1174, U'횡'},
    {1175, U'옹'},
    {1176, U'뻘'},
    {1177, U'옵'},
    {1178, U'옴'},
    {1179, U'얹'},
    {1180, U'쑥'},
    {1181, U'깄'},
    {1182, U'므'},
    {1183, U'찡'},
    {1184, U'젖'},
    {1185, U'꽈'},
    {1186, U'틸'},
    {1187, U'콕'},
    {1188, U'첩'},
    {1189, U'똘'},
    {1190, U'쿵'},
    {1191, U'왤'},
    {1192, U'괌'},
    {1193, U'밸'},
    {1194, U'녜'},
    {1195, U'갸'},
    {1196, U'펀'},
    {1197, U'칫'},
    {1198, U'맺'},
    {1199, U'탭'},
    {1200, U'쁨'},
    {1201, U'폈'},
    {1202, U'펼'},
    {1203, U'첼'},
    {1204, U'숱'},
    {1205, U'섰'},
    {1206, U'킥'},
    {1207, U'맑'},
    {1208, U'랗'},
    {1209, U'펐'},
    {1210, U'넛'},
    {1211, U'솜'},
    {1212, U'벙'},
    {1213, U'껑'},
    {1214, U'f'},
    {1215, U'룡'},
    {1216, U'훌'},
    {1217, U'x'},
    {1218, U'쓱'},
    {1219, U'늬'},
    {1220, U'곽'},
    {1221, U'y'},
    {1222, U'욘'},
    {1223, U'돔'},
    {1224, U'겄'},
    {1225, U'텝'},
    {1226, U'훠'},
    {1227, U'텅'},
    {1228, U'씌'},
    {1229, U'꺾'},
    {1230, U'벚'},
    {1231, U'렷'},
    {1232, U'귓'},
    {1233, U'찹'},
    {1234, U'툴'},
    {1235, U'깅'},
    {1236, U'쭤'},
    {1237, U'욜'},
    {1238, U'얌'},
    {1239, U'짖'},
    {1240, U'옳'},
    {1241, U'벳'},
    {1242, U'뛸'},
    {1243, U'깠'},
    {1244, U'퍽'},
    {1245, U'퀸'},
    {1246, U'엮'},
    {1247, U'삽'},
    {1248, U'겟'},
    {1249, U'왓'},
    {1250, U'댈'},
    {1251, U'샴'},
    {1252, U'뻗'},
    {1253, U'됨'},
    {1254, U'얜'},
    {1255, U'굵'},
    {1256, U'눕'},
    {1257, U'갇'},
    {1258, U'셰'},
    {1259, U'늫'},
    {1260, U'텨'},
    {1261, U'숍'},
    {1262, U'뻑'},
    {1263, U'됩'},
    {1264, U'잎'},
    {1265, U'뭇'},
    {1266, U'퐁'},
    {1267, U'팸'},
    {1268, U'쯔'},
    {1269, U'넜'},
    {1270, U'깍'},
    {1271, U'쌔'},
    {1272, U'셈'},
    {1273, U'읍'},
    {1274, U'픔'},
    {1275, U'펫'},
    {1276, U'콧'},
    {1277, U'얗'},
    {1278, U'눅'},
    {1279, U'j'},
    {1280, U'쬐'},
    {1281, U'렙'},
    {1282, U'닙'},
    {1283, U'슥'},
    {1284, U'흙'},
    {1285, U'쭝'},
    {1286, U'짭'},
    {1287, U'샹'},
    {1288, U'릏'},
    {1289, U'럿'},
    {1290, U'덧'},
    {1291, U'즙'},
    {1292, U'늑'},
    {1293, U'괄'},
    {1294, U'킷'},
    {1295, U'쿡'},
    {1296, U'캉'},
    {1297, U'둡'},
    {1298, U'톨'},
    {1299, U'엣'},
    {1300, U'숟'},
    {1301, U'낑'},
    {1302, U'펭'},
    {1303, U'왁'},
    {1304, U'쏴'},
    {1305, U'쏙'},
    {1306, U'봅'},
    {1307, U'멧'},
    {1308, U'줏'},
    {1309, U'뵈'},
    {1310, U'쫑'},
    {1311, U'륨'},
    {1312, U'h'},
    {1313, U'펄'},
    {1314, U'짼'},
    {1315, U'짚'},
    {1316, U'껐'},
    {1317, U'겜'},
    {1318, U'싯'},
    {1319, U'붐'},
    {1320, U'렐'},
    {1321, U'돗'},
    {1322, U'팥'},
    {1323, U'웰'},
    {1324, U'륜'},
    {1325, U'잣'},
    {1326, U'슝'},
    {1327, U'붉'},
    {1328, U'윽'},
    {1329, U'삘'},
    {1330, U'딲'},
    {1331, U'갯'},
    {1332, U'횐'},
    {1333, U'헨'},
    {1334, U'캘'},
    {1335, U'쩰'},
    {1336, U'뤘'},
    {1337, U'랴'},
    {1338, U'껜'},
    {1339, U'펠'},
    {1340, U'킵'},
    {1341, U'컹'},
    {1342, U'렘'},
    {1343, U'뛴'},
    {1344, U'헝'},
    {1345, U'씽'},
    {1346, U'뮬'},
    {1347, U'젯'},
    {1348, U'샜'},
    {1349, U'뿜'},
    {1350, U'뒹'},
    {1351, U'뎌'},
    {1352, U'깬'},
    {1353, U'챠'},
    {1354, U'왈'},
    {1355, U'뾰'},
    {1356, U'뚤'},
    {1357, U'꾹'},
    {1358, U'갛'},
    {1359, U'잌'},
    {1360, U'엿'},
    {1361, U'솥'},
    {1362, U'벡'},
    {1363, U'룻'},
    {1364, U'꿍'},
    {1365, U'곈'},
    {1366, U'팜'},
    {1367, U'튕'},
    {1368, U'컥'},
    {1369, U'첸'},
    {1370, U'줍'},
    {1371, U'섀'},
    {1372, U'몫'},
    {1373, U'뜸'},
    {1374, U'깁'},
    {1375, U'핬'},
    {1376, U'쭘'},
    {1377, U'쌰'},
    {1378, U'넬'},
    {1379, U'큘'},
    {1380, U'쾅'},
    {1381, U'캄'},
    {1382, U'괘'},
    {1383, U'쟀'},
    {1384, U'윌'},
    {1385, U'엌'},
    {1386, U'앓'},
    {1387, U'씁'},
    {1388, U'륭'},
    {1389, U'W'},
    {1390, U'쑤'},
    {1391, U'삥'},
    {1392, U'돕'},
    {1393, U'깰'},
    {1394, U'핍'},
    {1395, U'텃'},
    {1396, U'슛'},
    {1397, U'맸'},
    {1398, U'롬'},
    {1399, U'갭'},
    {1400, U'얽'},
    {1401, U'쏭'},
    {1402, U'랠'},
    {1403, U'겔'},
    {1404, U'Q'},
    {1405, U'핥'},
    {1406, U'킴'},
    {1407, U'읏'},
    {1408, U'앚'},
    {1409, U'숯'},
    {1410, U'밋'},
    {1411, U'뽐'},
    {1412, U'뻣'},
    {1413, U'눴'},
    {1414, U'잭'},
    {1415, U'뽈'},
    {1416, U'뗐'},
    {1417, U'꽝'},
    {1418, U'훑'},
    {1419, U'캥'},
    {1420, U'쫀'},
    {1421, U'뵙'},
    {1422, U'홋'},
    {1423, U'펍'},
    {1424, U'뺨'},
    {1425, U'됬'},
    {1426, U'끽'},
    {1427, U'빕'},
    {1428, U'밉'},
    {1429, U'꿋'},
    {1430, U'헉'},
    {1431, U'캣'},
    {1432, U'촘'},
    {1433, U'셌'},
    {1434, U'삔'},
    {1435, U'삑'},
    {1436, U'뽂'},
    {1437, U'뮌'},
    {1438, U'뗄'},
    {1439, U'텼'},
    {1440, U'탬'},
    {1441, U'쨍'},
    {1442, U'웜'},
    {1443, U'앰'},
    {1444, U'맴'},
    {1445, U'띡'},
    {1446, U'꿇'},
    {1447, U'걀'},
    {1448, U'흩'},
    {1449, U'쥴'},
    {1450, U'씸'},
    {1451, U'낡'},
    {1452, U'곁'},
    {1453, U'휙'},
    {1454, U'쿤'},
    {1455, U'켄'},
    {1456, U'츤'},
    {1457, U'얕'},
    {1458, U'썪'},
    {1459, U'둠'},
    {1460, U'촛'},
    {1461, U'챌'},
    {1462, U'죙'},
    {1463, U'쟈'},
    {1464, U'잰'},
    {1465, U'뵀'},
    {1466, U'w'},
    {1467, U'푠'},
    {1468, U'폿'},
    {1469, U'쨈'},
    {1470, U'밲'},
    {1471, U'랖'},
    {1472, U'떵'},
    {1473, U'찻'},
    {1474, U'옇'},
    {1475, U'뽁'},
    {1476, U'둣'},
    {1477, U'닳'},
    {1478, U'긱'},
    {1479, U'곶'},
    {1480, U'휠'},
    {1481, U'춧'},
    {1482, U'쐈'},
    {1483, U'썽'},
    {1484, U'뼛'},
    {1485, U'떳'},
    {1486, U'굘'},
    {1487, U'훗'},
    {1488, U'퀵'},
    {1489, U'봬'},
    {1490, U'릅'},
    {1491, U'꺠'},
    {1492, U'슉'},
    {1493, U'눔'},
    {1494, U'끙'},
    {1495, U'궐'},
    {1496, U'2'},
    {1497, U'촥'},
    {1498, U'젬'},
    {1499, U'솟'},
    {1500, U'맷'},
    {1501, U'룐'},
    {1502, U'뎃'},
    {1503, U'깽'},
    {1504, U'툼'},
    {1505, U'쎌'},
    {1506, U'쉼'},
    {1507, U'쉘'},
    {1508, U'숑'},
    {1509, U'뎀'},
    {1510, U'냔'},
    {1511, U'쫘'},
    {1512, U'쎘'},
    {1513, U'싣'},
    {1514, U'섣'},
    {1515, U'샾'},
    {1516, U'맬'},
    {1517, U'뗀'},
    {1518, U'꾀'},
    {1519, U'헹'},
    {1520, U'햐'},
    {1521, U'톰'},
    {1522, U'췌'},
    {1523, U'챈'},
    {1524, U'봔'},
    {1525, U'밧'},
    {1526, U'맏'},
    {1527, U'딥'},
    {1528, U'늠'},
    {1529, U'낵'},
    {1530, U'낱'},
    {1531, U'꺄'},
    {1532, U'갬'},
    {1533, U'훼'},
    {1534, U'핼'},
    {1535, U'튠'},
    {1536, U'웩'},
    {1537, U'쏜'},
    {1538, U'뿅'},
    {1539, U'빰'},
    {1540, U'딤'},
    {1541, U'꿉'},
    {1542, U'걜'},
    {1543, U'1'},
    {1544, U'짙'},
    {1545, U'얍'},
    {1546, U'샛'},
    {1547, U'뗘'},
    {1548, U'듭'},
    {1549, U'챘'},
    {1550, U'쯧'},
    {1551, U'짹'},
    {1552, U'잦'},
    {1553, U'옐'},
    {1554, U'빳'},
    {1555, U'몹'},
    {1556, U'몄'},
    {1557, U'똔'},
    {1558, U'딧'},
    {1559, U'놉'},
    {1560, U'궜'},
    {1561, U'굼'},
    {1562, U'헥'},
    {1563, U'캬'},
    {1564, U'챕'},
    {1565, U'쟨'},
    {1566, U'멓'},
    {1567, U'똠'},
    {1568, U'댐'},
    {1569, U'텁'},
    {1570, U'켈'},
    {1571, U'첵'},
    {1572, U'숄'},
    {1573, U'띨'},
    {1574, U'듦'},
    {1575, U'궤'},
    {1576, U'곗'},
    {1577, U'튈'},
    {1578, U'좆'},
    {1579, U'윷'},
    {1580, U'옅'},
    {1581, U'얏'},
    {1582, U'믈'},
    {1583, U'룽'},
    {1584, U'띃'},
    {1585, U'딕'},
    {1586, U'뎁'},
    {1587, U'닛'},
    {1588, U'냑'},
    {1589, U'겅'},
    {1590, U'휩'},
    {1591, U'팎'},
    {1592, U'틋'},
    {1593, U'콸'},
    {1594, U'콥'},
    {1595, U'잴'},
    {1596, U'웁'},
    {1597, U'슘'},
    {1598, U'멱'},
    {1599, U'랏'},
    {1600, U'떄'},
    {1601, U'뒨'},
    {1602, U'꿰'},
    {1603, U'깻'},
    {1604, U'긌'},
    {1605, U'젼'},
    {1606, U'윰'},
    {1607, U'웍'},
    {1608, U'앳'},
    {1609, U'샬'},
    {1610, U'샥'},
    {1611, U'볕'},
    {1612, U'멩'},
    {1613, U'넹'},
    {1614, U'넙'},
    {1615, U'끕'},
    {1616, U'휜'},
    {1617, U'텄'},
    {1618, U'쫒'},
    {1619, U'쩝'},
    {1620, U'쨋'},
    {1621, U'윳'},
    {1622, U'쉭'},
    {1623, U'쇽'},
    {1624, U'셧'},
    {1625, U'뵐'},
    {1626, U'땔'},
    {1627, U'덱'},
    {1628, U'댑'},
    {1629, U'꺽'},
    {1630, U'곪'},
    {1631, U'켔'},
    {1632, U'츰'},
    {1633, U'읜'},
    {1634, U'쑈'},
    {1635, U'볐'},
    {1636, U'및'},
    {1637, U'롷'},
    {1638, U'딛'},
    {1639, U'냇'},
    {1640, U'3'},
    {1641, U'혓'},
    {1642, U'팻'},
    {1643, U'팰'},
    {1644, U'킁'},
    {1645, U'촤'},
    {1646, U'쨰'},
    {1647, U'잿'},
    {1648, U'옌'},
    {1649, U'쐬'},
    {1650, U'쌋'},
    {1651, U'슁'},
    {1652, U'쉑'},
    {1653, U'빢'},
    {1654, U'뵌'},
    {1655, U'뭄'},
    {1656, U'묽'},
    {1657, U'뎠'},
    {1658, U'늪'},
    {1659, U'뇽'},
    {1660, U'훤'},
    {1661, U'횔'},
    {1662, U'홧'},
    {1663, U'햅'},
    {1664, U'푯'},
    {1665, U'팼'},
    {1666, U'탯'},
    {1667, U'탤'},
    {1668, U'큭'},
    {1669, U'짢'},
    {1670, U'읊'},
    {1671, U'롸'},
    {1672, U'띤'},
    {1673, U'놋'},
    {1674, U'넴'},
    {1675, U'귿'},
    {1676, U'q'},
    {1677, U'휑'},
    {1678, U'퓸'},
    {1679, U'튿'},
    {1680, U'튄'},
    {1681, U'촐'},
    {1682, U'쭌'},
    {1683, U'짊'},
    {1684, U'숴'},
    {1685, U'숀'},
    {1686, U'뿡'},
    {1687, U'뻬'},
    {1688, U'렜'},
    {1689, U'뗬'},
    {1690, U'늄'},
    {1691, U'끍'},
    {1692, U'곌'},
    {1693, U'갰'},
    {1694, U'폄'},
    {1695, U'콰'},
    {1696, U'캇'},
    {1697, U'캅'},
    {1698, U'늉'},
    {1699, U'뉜'},
    {1700, U'큔'},
    {1701, U'콱'},
    {1702, U'켠'},
    {1703, U'쳔'},
    {1704, U'쳇'},
    {1705, U'읔'},
    {1706, U'읎'},
    {1707, U'욤'},
    {1708, U'뼘'},
    {1709, U'롹'},
    {1710, U'렝'},
    {1711, U'뚠'},
    {1712, U'땋'},
    {1713, U'덨'},
    {1714, U'넵'},
    {1715, U'넝'},
    {1716, U'넋'},
    {1717, U'꿩'},
    {1718, U'꼿'},
    {1719, U'깟'},
    {1720, U'곯'},
    {1721, U'4'},
    {1722, U'힝'},
    {1723, U'헀'},
    {1724, U'푤'},
    {1725, U'쿱'},
    {1726, U'캤'},
    {1727, U'챗'},
    {1728, U'쯩'},
    {1729, U'쮸'},
    {1730, U'읗'},
    {1731, U'윅'},
    {1732, U'얠'},
    {1733, U'씰'},
    {1734, U'썅'},
    {1735, U'쌜'},
    {1736, U'쌉'},
    {1737, U'슌'},
    {1738, U'쉿'},
    {1739, U'쇳'},
    {1740, U'셍'},
    {1741, U'뿍'},
    {1742, U'뼌'},
    {1743, U'뺌'},
    {1744, U'밈'},
    {1745, U'룟'},
    {1746, U'룔'},
    {1747, U'뜀'},
    {1748, U'끅'},
    {1749, U'꾜'},
    {1750, U'겡'},
    {1751, U'Z'},
    {1752, U'횰'},
    {1753, U'헴'},
    {1754, U'핌'},
    {1755, U'펩'},
    {1756, U'펨'},
    {1757, U'켸'},
    {1758, U'쩠'},
    {1759, U'잽'},
    {1760, U'엡'},
    {1761, U'앝'},
    {1762, U'쓕'},
    {1763, U'썜'},
    {1764, U'쌘'},
    {1765, U'삣'},
    {1766, U'빻'},
    {1767, U'몀'},
    {1768, U'뤼'},
    {1769, U'롄'},
    {1770, U'떫'},
    {1771, U'덫'},
    {1772, U'뉸'},
    {1773, U'꽥'},
    {1774, U'궂'},
    {1775, U'괭'},
    {1776, U'0'},
    {1777, U'힉'},
    {1778, U'휸'},
    {1779, U'퓰'},
    {1780, U'퉤'},
    {1781, U'퀭'},
    {1782, U'켐'},
    {1783, U'췻'},
    {1784, U'챡'},
    {1785, U'쨀'},
    {1786, U'젱'},
    {1787, U'읒'},
    {1788, U'웡'},
    {1789, U'옙'},
    {1790, U'얬'},
    {1791, U'앎'},
    {1792, U'씼'},
    {1793, U'쑨'},
    {1794, U'쐐'},
    {1795, U'쌨'},
    {1796, U'쉈'},
    {1797, U'숩'},
    {1798, U'셤'},
    {1799, U'삠'},
    {1800, U'뱄'},
    {1801, U'뭥'},
    {1802, U'멨'},
    {1803, U'먀'},
    {1804, U'랒'},
    {1805, U'땟'},
    {1806, U'땁'},
    {1807, U'뉠'},
    {1808, U'꽌'},
    {1809, U'귐'},
    {1810, U'굥'},
    {1811, U'굣'},
    {1812, U'겋'},
    {1813, U'8'},
    {1814, U'5'},
    {1815, U'훙'},
    {1816, U'혠'},
    {1817, U'폔'},
    {1818, U'튬'},
    {1819, U'퉜'},
    {1820, U'탉'},
    {1821, U'큽'},
    {1822, U'퀀'},
    {1823, U'쿰'},
    {1824, U'켤'},
    {1825, U'켕'},
    {1826, U'칡'},
    {1827, U'쯘'},
    {1828, U'짯'},
    {1829, U'짔'},
    {1830, U'쥔'},
    {1831, U'줜'},
    {1832, U'죗'},
    {1833, U'욧'},
    {1834, U'쒸'},
    {1835, U'쏼'},
    {1836, U'쌂'},
    {1837, U'셴'},
    {1838, U'샨'},
    {1839, U'뽜'},
    {1840, U'뻠'},
    {1841, U'빝'},
    {1842, U'봥'},
    {1843, U'뫼'},
    {1844, U'맜'},
    {1845, U'맀'},
    {1846, U'뤠'},
    {1847, U'띌'},
    {1848, U'띈'},
    {1849, U'뛌'},
    {1850, U'뚸'},
    {1851, U'떽'},
    {1852, U'떰'},
    {1853, U'떈'},
    {1854, U'땍'},
    {1855, U'돠'},
    {1856, U'뎄'},
    {1857, U'늣'},
    {1858, U'뇸'},
    {1859, U'놥'},
    {1860, U'녈'},
    {1861, U'넒'},
    {1862, U'낏'},
    {1863, U'갉'},
    {1864, U'z'},
    {1865, U'힛'},
    {1866, U'휀'},
    {1867, U'헙'},
    {1868, U'픗'},
    {1869, U'퐈'},
    {1870, U'팹'},
    {1871, U'틔'},
    {1872, U'퇸'},
    {1873, U'텟'},
    {1874, U'탰'},
    {1875, U'퀼'},
    {1876, U'칬'},
    {1877, U'췰'},
    {1878, U'쳥'},
    {1879, U'챂'},
    {1880, U'찧'},
    {1881, U'쭙'},
    {1882, U'쬬'},
    {1883, U'쩄'},
    {1884, U'쨉'},
    {1885, U'줴'},
    {1886, U'죵'},
    {1887, U'좍'},
    {1888, U'좃'},
    {1889, U'읐'},
    {1890, U'읃'},
    {1891, U'웟'},
    {1892, U'왯'},
    {1893, U'옭'},
    {1894, U'씅'},
    {1895, U'쒀'},
    {1896, U'쑹'},
    {1897, U'쏵'},
    {1898, U'쎔'},
    {1899, U'쎅'},
    {1900, U'슾'},
    {1901, U'쉣'},
    {1902, U'솨'},
    {1903, U'셸'},
    {1904, U'셩'},
    {1905, U'삯'},
    {1906, U'뿟'},
    {1907, U'뾱'},
    {1908, U'뽝'},
    {1909, U'뻰'},
    {1910, U'뵤'},
    {1911, U'볏'},
    {1912, U'뱌'},
    {1913, U'뱁'},
    {1914, U'밨'},
    {1915, U'밎'},
    {1916, U'믐'},
    {1917, U'뭡'},
    {1918, U'뭠'},
    {1919, U'뭍'},
    {1920, U'묭'},
    {1921, U'뫠'},
    {1922, U'멏'},
    {1923, U'멎'},
    {1924, U'맽'},
    {1925, U'뙤'},
    {1926, U'떔'},
    {1927, U'땃'},
    {1928, U'덷'},
    {1929, U'댜'},
    {1930, U'늰'},
    {1931, U'놘'},
    {1932, U'냘'},
    {1933, U'뀜'},
    {1934, U'꽹'},
    {1935, U'꽐'},
    {1936, U'꼇'},
    {1937, U'껸'},
    {1938, U'껨'},
    {1939, U'꺅'},
    {1940, U'곘'},
    {1941, U'겝'},
    {1942, U'흣'},
    {1943, U'흝'},
    {1944, U'흄'},
    {1945, U'헸'},
    {1946, U'헜'},
    {1947, U'햘'},
    {1948, U'햑'},
    {1949, U'핟'},
    {1950, U'픙'},
    {1951, U'픕'},
    {1952, U'푀'},
    {1953, U'퐉'},
    {1954, U'틑'},
    {1955, U'틍'},
    {1956, U'큣'},
    {1957, U'퀏'},
    {1958, U'콴'},
    {1959, U'켁'},
    {1960, U'칟'},
    {1961, U'츳'},
    {1962, U'츨'},
    {1963, U'츈'},
    {1964, U'쵝'},
    {1965, U'촙'},
    {1966, U'찦'},
    {1967, U'찟'},
    {1968, U'쭁'},
    {1969, U'쫏'},
    {1970, U'쩬'},
    {1971, U'쩧'},
    {1972, U'짆'},
    {1973, U'쥰'},
    {1974, U'쥘'},
    {1975, U'좐'},
    {1976, U'졍'},
    {1977, U'쟝'},
    {1978, U'잧'},
    {1979, U'잍'},
    {1980, U'욍'},
    {1981, U'왬'},
    {1982, U'옫'},
    {1983, U'옜'},
    {1984, U'옉'},
    {1985, U'엤'},
    {1986, U'얐'},
    {1987, U'얉'},
    {1988, U'쒯'},
    {1989, U'쑐'},
    {1990, U'쏸'},
    {1991, U'썻'},
    {1992, U'쌕'},
    {1993, U'싷'},
    {1994, U'솝'},
    {1995, U'솎'},
    {1996, U'샅'},
    {1997, U'삻'},
    {1998, U'쁩'},
    {1999, U'빘'},
    {2000, U'뷸'},
    {2001, U'뷴'},
    {2002, U'봽'},
    {2003, U'봈'},
    {2004, U'볌'},
    {2005, U'벋'},
    {2006, U'믕'},
    {2007, U'뮴'},
    {2008, U'뭬'},
    {2009, U'뭑'},
    {2010, U'묜'},
    {2011, U'맇'},
    {2012, U'릈'},
    {2013, U'륑'},
    {2014, U'랮'},
    {2015, U'랟'},
    {2016, U'띔'},
    {2017, U'떱'},
    {2018, U'듈'},
    {2019, U'됴'},
    {2020, U'됫'},
    {2021, U'됙'},
    {2022, U'됑'},
    {2023, U'댤'},
    {2024, U'닯'},
    {2025, U'늗'},
    {2026, U'뉩'},
    {2027, U'눟'},
    {2028, U'눗'},
    {2029, U'넚'},
    {2030, U'냡'},
    {2031, U'낢'},
    {2032, U'꾿'},
    {2033, U'꼳'},
    {2034, U'꼄'},
    {2035, U'겊'},
    {2036, U'갼'},
    {2037, U'6'},
};
