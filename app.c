#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "vsimplefs.h"
void create(){
    int ret;
    
    printf ("started\n"); 
    
    ret  = create_format_vdisk ("example", 21); 
    if (ret != 0) {
	printf ("there was an error in creating the disk\n");
	exit(1); 
    }

    printf ("disk created and formatted.\n"); 
}

int main(int argc, char **argv)
{
    char* text5000 = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Morbi convallis laoreet rhoncus. Pellentesque ac blandit nibh, ac pulvinar ipsum. Donec sollicitudin, velit nec scelerisque varius, elit sapien porta sem, nec facilisis lectus dui nec est. Integer quis laoreet ex, at mollis diam. Quisque id nisi sed sapien tempus eleifend. Sed tempor, nulla a rutrum tempor, velit felis vestibulum magna, vitae consectetur purus nunc non libero. Cras fermentum nunc at odio porta, ac pretium purus vestibulum. Ut hendrerit feugiat justo vitae aliquet. Donec pulvinar elit eu turpis malesuada posuere. Phasellus metus ligula, aliquam luctus mauris at, efficitur convallis nulla. Vivamus turpis elit, mollis luctus massa quis, commodo tristique quam. Vivamus eu sollicitudin elit. Aenean a interdum orci. Pellentesque habitant morbi tristique senectus et netus et malesuada fames ac turpis egestas. Fusce orci velit, fermentum vel risus id, imperdiet efficitur libero. Mauris hendrerit ipsum nec volutpat pretium.Etiam feugiat magna arcu, ac tincidunt massa eleifend scelerisque. Nullam turpis nisl, convallis interdum tempus eget, pulvinar id est. Integer malesuada consectetur velit, efficitur convallis turpis suscipit vitae. Aenean faucibus sem in egestas bibendum. Quisque tempus faucibus metus a consequat. Fusce venenatis semper turpis vel pellentesque. Pellentesque habitant morbi tristique senectus et netus et malesuada fames ac turpis egestas. Vestibulum ante ipsum primis in faucibus orci luctus et ultrices posuere cubilia curae; Donec egestas tellus et eros dignissim, non imperdiet ante faucibus. Aliquam erat volutpat. Sed vitae lacus vitae leo euismod euismod. Nullam aliquam neque justo, quis venenatis elit placerat nec. Vivamus et ipsum tincidunt, congue nisi eu, interdum turpis.Cras in varius nulla, eget venenatis enim. Pellentesque quis augue feugiat lectus pellentesque tristique. Nam lorem sem, congue vitae arcu in, laoreet imperdiet massa. Ut semper tincidunt elit, in tincidunt purus dignissim sed. Donec pellentesque quam id sodales eleifend. Suspendisse sodales, erat vel euismod ornare, orci nisl fermentum sem, at vestibulum nisl massa vitae nisi. Praesent arcu lectus, feugiat ut lectus quis, dapibus varius nulla. Vestibulum nec sem turpis. Curabitur dignissim a sem eu pharetra. Aenean pharetra blandit volutpat. Cras mattis ultricies ex ut vulputate. Aenean egestas augue vitae nunc maximus euismod. Vestibulum a ligula sed massa pharetra fermentum eget ac libero. Aenean ultrices, neque non aliquet rhoncus, sem ex laoreet purus, id elementum tortor nisl non nulla. Nunc sodales massa ut odio pellentesque, ut gravida ligula facilisis.Suspendisse potenti. Maecenas pellentesque turpis vitae lectus ornare porttitor. Nam scelerisque bibendum quam at luctus. Etiam nisl sapien, auctor vitae ullamcorper eu, pellentesque eget massa. Orci varius natoque penatibus et magnis dis parturient montes, nascetur ridiculus mus. Maecenas interdum iaculis sem, vel commodo metus accumsan sed. Mauris molestie nisl leo, non accumsan diam bibendum in. Aenean hendrerit in libero ut pretium. Maecenas nec gravida turpis. Quisque enim est, commodo eu ipsum eget, vulputate aliquam nibh. Nam semper ut ligula vitae finibus. Vestibulum auctor faucibus ultrices. Ut aliquam neque in tempus vestibulum. Quisque congue pretium lorem, at hendrerit tortor vestibulum in. Phasellus ligula nisl, aliquam ac mauris in, porta rutrum sem. Etiam molestie auctor massa, in ultrices tellus rutrum nec.Pellentesque habitant morbi tristique senectus et netus et malesuada fames ac turpis egestas. Phasellus sodales pretium pulvinar. Duis tincidunt purus vitae sem pharetra aliquet ut nec lorem. Aenean ullamcorper tellus eget erat aliquam egestas. Vestibulum porta tortor ut sodales faucibus. Maecenas nec turpis vitae mi placerat sodales. Quisque dictum lacus quis dolor hendrerit blandit. Vivamus tempus eget lorem et tristique. Sed faucibus purus sed eros ultricies ornare. Phasellus quis finibus velit. Nam eu lacus mollis, pretium eros vel, posuere purus.Morbi vestibulum ultricies nunc non fringilla. Phasellus condimentum ut est a interdum. Nullam laoreet neque vel turpis sodales gravida. Integer eu lacus odio. Integer ut diam porta, tempus tellus a, euismod lectus. Vestibulum vel ipsum in elit tincidunt dignissim. Mauris vitae auctor orci. Vivamus ultricies justo metus, sollicitudin egestas justo vehicula eget. Mauris eget risus urna. Cras blandit elit sit amet augue iaculis, sit amet ultricies lorem sollicitudin. Nunc ut lectus mollis, ullamcorper tortor ut, tincidunt nulla.Praesent gravida tristique vehicula. Nunc aliquam tellus nec turpis semper, ut pellentesque leo interdum. Integer tortor turpis, gravida sed ultricies non, dapibus sit amet sem. Maecenas malesuada augue nibh, non hendrerit dui vulputate ac. Donec molestie sed nibh id accumsan. Nunc convallis felis magna, ac convallis metus facilisis ac. In hac habitasse platea dictumst. Aliquam pretium, elit eget finibus sapien.";
    //char* text10000 = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Mauris sollicitudin sollicitudin tellus in elementum. Cras congue consectetur lacus, ac fermentum enim dictum eget. Integer accumsan, erat in mattis posuere, tellus lectus congue dolor, sit amet mattis justo ante at nibh. Vivamus diam erat, commodo et massa in, vehicula ornare quam. Nulla pellentesque porttitor nisl sit amet condimentum. Maecenas mattis sapien et tincidunt interdum. Aliquam vehicula nisl id rhoncus lacinia.Etiam urna est, cursus sit amet mollis non, pulvinar sed libero. Fusce eu orci ut elit rhoncus ultrices vel ac ex. Vestibulum id gravida tortor. Suspendisse iaculis pharetra nisl tristique aliquet. Curabitur magna risus, laoreet euismod fringilla at, blandit ac risus. Morbi ac scelerisque ex. Donec in finibus purus, non pellentesque eros. Fusce placerat erat ut venenatis tempor. Duis et nibh velit. Aliquam erat volutpat. Vestibulum ultrices augue risus, vel tempor sem ullamcorper sit amet. Curabitur neque lectus, venenatis ut est eget, sollicitudin pellentesque justo. Praesent vehicula eros condimentum iaculis porta. Nulla eget lorem congue, dictum dolor nec, mattis justo.Ut vel quam sem. Phasellus nec sapien est. Vestibulum non euismod nulla. Integer aliquam ultricies pharetra. Ut ultricies odio auctor tristique fermentum. Fusce interdum urna velit, vel feugiat velit convallis quis. Proin finibus nunc justo, ac euismod nunc cursus id.Donec convallis neque at nisi bibendum, et ultricies sem sodales. Mauris sit amet magna laoreet, consectetur dolor quis, scelerisque ex. Fusce a mauris quam. Nunc sit amet arcu ut mauris vehicula fringilla. Morbi at ullamcorper quam. Mauris fermentum dolor ante, in ullamcorper nulla venenatis non. Sed vulputate commodo sapien, sit amet imperdiet nunc consequat nec. Aenean commodo placerat nibh commodo rhoncus.Donec nibh eros, facilisis et lectus at, luctus luctus nisl. Vivamus quis laoreet nisl. Nam pretium libero ut neque aliquam vulputate. Etiam ornare risus a dui interdum, eget convallis ex tincidunt. Nam interdum est eget est elementum sollicitudin. Aenean rhoncus facilisis urna, non aliquam mauris venenatis et. Duis dapibus nunc tincidunt sollicitudin rhoncus. Etiam a elit non nunc viverra sodales. Proin nulla felis, tincidunt in vehicula pretium, suscipit nec ipsum. Curabitur eleifend risus vitae mi eleifend, placerat blandit est ultrices. Donec sem elit, tempus aliquam enim sit amet, imperdiet ultricies velit.Aenean finibus vitae ligula at vehicula. Pellentesque habitant morbi tristique senectus et netus et malesuada fames ac turpis egestas. Maecenas vel neque quis nunc tempus viverra. Nam blandit nisi nec eros faucibus, vitae faucibus libero semper. Nam maximus felis lacinia, finibus erat bibendum, mollis est. Donec sed odio mauris. Nam et commodo ex, et imperdiet lectus. Duis sodales ligula massa, nec gravida lectus convallis vitae. Vivamus purus dui, sollicitudin vitae libero elementum, gravida iaculis risus. Integer porttitor elit nisl, vel aliquam arcu efficitur quis. Proin nec mauris dignissim, rhoncus velit congue, ultrices diam. Lorem ipsum dolor sit amet, consectetur adipiscing elit. Duis maximus, mi id vehicula accumsan, turpis justo sollicitudin magna, sed consectetur erat sem commodo tellus. Donec non felis in turpis porta facilisis eu in risus. Donec at aliquet leo.Proin ante diam, sollicitudin non auctor eu, pretium eu justo. Ut vitae dapibus sem, quis vehicula metus. In ultrices efficitur dui, lacinia vestibulum massa pulvinar at. Vestibulum ante ipsum primis in faucibus orci luctus et ultrices posuere cubilia curae; Aenean eu tincidunt justo. Nam vitae arcu non mi fringilla pretium sit amet ac nisi. Fusce pharetra purus sapien, eget auctor felis laoreet sed. Morbi iaculis pretium scelerisque. Aliquam felis diam, eleifend in fermentum at, porttitor in mauris. Vestibulum porta commodo tortor sit amet consectetur. Suspendisse sagittis sollicitudin pellentesque. Suspendisse vitae dolor diam. Suspendisse nec nisl mollis, auctor velit a, vulputate ligula. Proin mattis posuere urna, ac eleifend massa vulputate quis. Curabitur tincidunt odio eget faucibus commodo. Phasellus lobortis ut mauris et consequat.Pellentesque sollicitudin rhoncus dui a tempor. Curabitur eleifend, turpis gravida posuere lacinia, lectus tellus ornare tellus, aliquet tincidunt augue dui et orci. Nam vestibulum nulla arcu, porta vulputate elit cursus nec. Suspendisse mauris eros, maximus eu congue nec, auctor ut risus. Vivamus non urna porttitor, porta urna eu, sodales urna. Praesent id tellus sed sem fringilla suscipit vitae et odio. Fusce euismod elit eu dictum dignissim. Donec ornare libero non viverra condimentum. Duis sed suscipit ligula. Maecenas tincidunt mattis interdum. Etiam lacinia nec dolor at blandit. Donec mattis eros mi. Fusce bibendum blandit justo, ac euismod mauris varius sit amet. Nullam ultrices at urna vehicula dictum. In hac habitasse platea dictumst.Cras placerat sem eget turpis eleifend, vestibulum facilisis neque dapibus. Integer luctus faucibus tortor, sit amet tincidunt est lacinia pellentesque. Vivamus ut nisl est. Phasellus sit amet arcu id lacus vulputate dictum. Sed dapibus tempus diam sit amet rhoncus. Praesent tincidunt lorem diam, in sagittis nunc dignissim et. Praesent est leo, interdum quis mollis a, fermentum at nunc.Cras pharetra tellus vulputate ligula malesuada dignissim. Fusce molestie, felis eget tempus fringilla, arcu leo placerat purus, a faucibus velit libero ac mi. Suspendisse potenti. Nulla in enim ut nisi pharetra ultricies. Suspendisse accumsan sed dolor quis iaculis. Donec non tempus tortor. Sed ut nisl cursus, scelerisque mi vitae, cursus odio.Nam eu enim nibh. Nam ornare ex nec neque rutrum facilisis. Proin commodo non orci et tristique. Maecenas interdum libero non ante interdum, sed tincidunt dolor rhoncus. Integer hendrerit lectus quis commodo placerat. Duis posuere sed leo non viverra. Duis in hendrerit augue. Aenean euismod tortor arcu, at vehicula est suscipit ut. Aenean in consectetur felis.Curabitur consectetur ac lectus rhoncus tincidunt. Cras placerat quam id elit facilisis ornare. Duis rhoncus libero dapibus, porttitor dolor sed, laoreet elit. Aenean lacinia posuere turpis, vel sodales leo sollicitudin vel. Morbi finibus lectus id luctus ullamcorper. Orci varius natoque penatibus et magnis dis parturient montes, nascetur ridiculus mus. Duis velit orci, tempus vel orci non, placerat fringilla ex. Suspendisse ultricies fringilla diam, sit amet vehicula quam mattis eu. Nam efficitur diam id est finibus porttitor. Morbi dictum blandit leo eu mattis. Phasellus lacinia ac velit nec interdum. Quisque nibh mauris, mattis hendrerit posuere ac, egestas a quam. Mauris lorem lorem, facilisis non sollicitudin ac, rhoncus at dui. Aenean eu felis ultricies arcu iaculis aliquam. Duis gravida arcu at commodo porta.Pellentesque ipsum mauris, tincidunt ac turpis sed, rhoncus tristique ex. Sed convallis mi at libero tincidunt pellentesque. Sed convallis odio ac semper auctor. Nunc vehicula mauris felis, at rutrum elit ornare ut. Donec eros mi, cursus ac justo lobortis, accumsan luctus libero. Aenean nec imperdiet dolor. Mauris nec nibh eu nunc luctus sollicitudin. Aliquam in porttitor massa, nec tristique libero. Mauris a leo ut diam semper pulvinar eget vel metus. Fusce volutpat fringilla metus. Nunc volutpat posuere ex vel malesuada. Ut efficitur justo mauris, quis lobortis ligula euismod sed. Donec pellentesque, nisl sed gravida luctus, magna dolor luctus dui, eu efficitur lorem nunc in dolor. Pellentesque eget volutpat mi. Aliquam vestibulum neque libero, eget porta tellus efficitur eleifend. Phasellus luctus nunc id turpis dictum, sed ultrices sem rutrum.Praesent vitae est ex. Aliquam erat volutpat. Nam eget risus risus. Maecenas vel malesuada velit, eleifend cursus tortor. Quisque sagittis quam in consequat ullamcorper. Pellentesque quis nulla lectus. Praesent lobortis tristique magna non varius.Sed condimentum metus et urna molestie vulputate. Donec imperdiet est vel nisl dictum, faucibus pharetra mi facilisis. Proin nec faucibus nisi, id vestibulum velit. Nulla a leo venenatis, dignissim libero ac, euismod purus. Aenean interdum velit pellentesque cursus dapibus. Nunc non magna ut metus bibendum semper id a tellus. Maecenas consequat lectus vel mauris ullamcorper lobortis. Fusce sagittis suscipit tortor, nec tristique erat auctor in. Ut porttitor viverra lectus, ut cursus sem varius malesuada. Vestibulum interdum dolor sit amet ante feugiat, sit amet accumsan arcu laoreet. Donec ut lacus in ex blandit egestas. Interdum et malesuada fames ac ante ipsum primis in faucibus. Nullam pulvinar tempor sapien, quis vestibulum risus commodo id. Aliquam a erat tellus.Phasellus ligula elit, lacinia vel dui eu, laoreet sagittis neque. Aliquam efficitur purus ultricies turpis maximus auctor. In neque magna, molestie at eros vitae, pellentesque facilisis dui. Nulla feugiat tempor justo id porta. Proin pharetra pellentesque fermentum. Donec a erat sit amet sapien porttitor efficitur. Sed lacinia diam dolor, quis venenatis ex iaculis in. Aliquam quis dui auctor, accumsan felis sed, venenatis diam. Maecenas et diam erat. Phasellus vulputate libero felis, sit amet accumsan turpis tristique commodo. Phasellus luctus, lacus at semper consequat, nulla tortor feugiat lacus, sed vulputate augue justo eleifend ante. Nullam turpis ipsum, hendrerit id aliquam eu, facilisis vel massa.Praesent sed enim at erat ullamcorper tempor. Aenean luctus lectus id mi fringilla dictum eget sit amet leo. Maecenas laoreet sed sapien sagittis sollicitudin. Sed faucibus fringilla mattis. Mauris nec lacus finibus, tristique diam vel, gravida felis. In lacus mauris, consectetur in vehicula id, maximus nec arcu. Integer turpis ex, pretium sit amet augue nec, iaculis eleifend lectus. Duis in massa semper velit feugiat porttitor. Vestibulum tristique ultricies elementum. Curabitur vestibulum porta euismod. Proin leo.";
    char* text7000 = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Aenean eget lorem eu purus feugiat rhoncus. Vivamus faucibus varius suscipit. Donec et magna et enim maximus pellentesque. Suspendisse porta ex est, eu sodales est efficitur convallis. Interdum et malesuada fames ac ante ipsum primis in faucibus. Pellentesque id eros tellus. Morbi sed dignissim eros. Pellentesque elementum odio nibh, id consequat tortor vestibulum eu. Suspendisse varius, orci et hendrerit feugiat, leo diam sollicitudin diam, vel porta ex ante at odio. Ut congue elit eget ligula laoreet vestibulum. Curabitur cursus nulla in mi convallis pulvinar. Vivamus lobortis condimentum dignissim. Donec sit amet libero quam. Proin non ante dignissim, finibus tortor euismod, ornare nibh. Nullam sit amet malesuada tortor.Phasellus vitae odio euismod, euismod lectus nec, rhoncus nisl. Praesent sit amet varius nisl, nec tempus magna. Etiam non egestas tellus. Nunc ut metus nunc. Suspendisse pulvinar odio non augue tempus, in laoreet massa imperdiet. Donec lectus sapien, scelerisque ac pharetra eleifend, facilisis ut magna. Phasellus sollicitudin ornare velit a vulputate. In vitae ante pellentesque leo pharetra venenatis. Nunc consectetur, purus at lobortis posuere, nisl nisl euismod nulla, sed accumsan erat dolor condimentum risus. In consectetur ligula in eros pharetra consequat. Aenean sed justo sit amet eros suscipit dignissim. Curabitur commodo sed est eu vestibulum. Integer vulputate massa blandit est aliquam pellentesque. Maecenas mi lacus, rutrum vitae aliquet in, pulvinar sit amet libero. Donec auctor velit in ligula dictum, in consequat nibh tristique.Mauris id vulputate velit. Ut ac purus vel dui euismod tempus. Nunc augue risus, ultricies quis risus facilisis, sollicitudin fermentum lectus. In dolor turpis, pretium id lacinia ut, hendrerit nec metus. Nulla aliquet, nibh sed tempus molestie, tellus nunc volutpat arcu, in consequat felis nisi sit amet orci. Duis consequat ut enim vitae malesuada. Donec accumsan congue mi ut rhoncus. Integer consectetur libero nec blandit pharetra. In lobortis scelerisque mauris, sit amet accumsan risus sollicitudin eget.Nulla sit amet dui nunc. Phasellus eget metus ac tellus semper vulputate eget sit amet sapien. In a tincidunt mauris. Ut semper, diam quis lobortis vehicula, nisi neque lobortis elit, et malesuada tortor mi ac neque. Maecenas consectetur libero a mattis ultrices. Sed condimentum convallis erat, sed porttitor tortor rhoncus vitae. Duis iaculis ullamcorper odio, vel feugiat ante imperdiet a. Phasellus turpis mauris, venenatis at rhoncus sed, bibendum eget sapien. Etiam id mauris purus. Donec eu mauris nulla. Vivamus malesuada aliquam est, non tincidunt arcu fringilla at. Curabitur tincidunt tellus varius vulputate fringilla. Pellentesque habitant morbi tristique senectus et netus et malesuada fames ac turpis egestas. Mauris accumsan neque nunc, in facilisis felis laoreet at.Maecenas consequat placerat nunc, a vestibulum lacus tincidunt ac. Etiam lobortis sapien ut sem viverra tempus. Praesent vulputate eros interdum, finibus magna sed, dapibus sem. Nulla pretium volutpat nulla vel convallis. Fusce nunc ligula, vehicula gravida lacus vel, fringilla vehicula ex. Integer vulputate, felis eget commodo commodo, risus sapien pulvinar arcu, sit amet mollis felis nisi sit amet quam. Fusce accumsan lorem mauris, vitae maximus libero scelerisque eu. Nullam posuere elementum lacus, non vehicula mauris interdum tristique. Quisque ac neque felis. Fusce condimentum ligula egestas dolor faucibus consectetur. Donec maximus est vel fringilla sollicitudin. Vestibulum euismod, justo at lacinia sollicitudin, dui lectus sollicitudin tortor, sit amet hendrerit dolor urna rhoncus libero. Donec tincidunt magna sem, ac egestas quam sollicitudin eu.Nam faucibus tempus leo a tempor. Etiam sed metus ut sapien dictum mattis sit amet et quam. Nullam rutrum justo sapien, sit amet sodales tortor porta non. Nam sed lectus accumsan nulla congue pellentesque nec non enim. Suspendisse ultricies enim in molestie fringilla. Donec vel elementum felis. Proin nec justo condimentum nunc rutrum blandit eget eleifend odio. Praesent bibendum pulvinar enim, non dictum nunc aliquam eget. Proin ultricies nec risus id scelerisque.Nunc efficitur est eget augue lacinia, vel interdum libero sollicitudin. Aliquam tempor odio nisi, ac euismod odio mattis nec. Cras suscipit pharetra augue, id viverra purus suscipit eu. Curabitur molestie turpis libero, et tempus lorem congue imperdiet. Ut dignissim lacus in mollis dictum. Praesent dignissim sed sem ac congue. Integer vel pellentesque arcu. Mauris tempus venenatis turpis. Maecenas ac elit at nibh pretium convallis.Donec bibendum iaculis massa non lacinia. Proin fringilla orci nec mi rhoncus viverra. Nullam ac velit non ex viverra blandit. In vitae tristique felis. Cras elementum leo ut placerat aliquam. Donec dapibus mi auctor nisl ornare, non egestas quam convallis. Proin in efficitur est, eget semper augue.Nulla viverra mollis enim, in fringilla magna laoreet mattis. Proin ut aliquet elit. Lorem ipsum dolor sit amet, consectetur adipiscing elit. Quisque efficitur luctus pretium. Donec lobortis lorem at malesuada facilisis. Sed nec nisi vehicula nisi consectetur dictum ac a felis. Donec et dolor ligula. Mauris porta vehicula quam, quis aliquet lacus maximus non. Donec scelerisque lacus non tellus sollicitudin, pulvinar fermentum magna tincidunt. Fusce bibendum a sem vel aliquam. Suspendisse facilisis vitae erat vulputate faucibus.Orci varius natoque penatibus et magnis dis parturient montes, nascetur ridiculus mus. Vestibulum ante ipsum primis in faucibus orci luctus et ultrices posuere cubilia curae; Morbi ante felis, dapibus in dui quis, vulputate molestie leo. Donec a nulla sit amet enim interdum aliquet. Nullam mattis vulputate massa, non suscipit tortor imperdiet vel. Aenean lacinia tincidunt nunc id facilisis. In hac habitasse platea dictumst. Quisque eu vulputate tortor. In accumsan ipsum vel dolor sodales, a ornare sapien ultrices. Quisque et cursus leo. Praesent id dolor eu lorem ultrices eleifend ut vitae purus.Cras ipsum leo, rutrum non dolor sit amet, auctor tincidunt enim. Etiam rhoncus urna ac maximus tempor. Nullam commodo, quam in scelerisque iaculis, tortor augue laoreet turpis, a volutpat mi orci vel leo. Duis feugiat molestie quam at convallis. Praesent id justo pretium metus elementum facilisis. Fusce fermentum, odio eu tempus eleifend, augue lorem blandit ligula, eget porttitor ante nibh eu felis. Praesent finibus lorem sit amet magna pretium molestie. Aenean posuere felis nibh, vel commodo nunc vulputate id. Ut mattis venenatis urna vel vestibulum. Mauris nec lacus sodales, cursus elit at, molestie arcu. Nullam lacus turpis, pharetra quis tortor vitae, suscipit finibus arcu. Class aptent taciti sociosqu ad litora torquent per conubia nostra, per inceptos himenaeos. Curabitur vehicula dapibus eros eget fermentum. Vivamus vel orci in morbi.";
    char* text3000 = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nullam malesuada justo ac lectus molestie convallis. Quisque id justo eget urna varius convallis. Curabitur euismod condimentum gravida. Donec at viverra arcu. Etiam commodo, est in malesuada elementum, erat ligula consectetur augue, vitae efficitur nisl nisl ut justo. Quisque rutrum neque nec ipsum dignissim, sed aliquet neque mollis. In pharetra viverra nulla nec venenatis. Aliquam turpis nisi, dignissim at odio sit amet, feugiat bibendum leo. Maecenas sed posuere turpis. Phasellus aliquet velit nec urna luctus, sed suscipit sapien imperdiet. Aenean congue dignissim lobortis.Morbi in tortor quis magna pulvinar pretium quis vel augue. Morbi id imperdiet urna. Suspendisse faucibus tortor quis enim tristique mollis in tincidunt quam. Duis id sem varius, eleifend lorem vulputate, dignissim nisi. Praesent consectetur ex sed bibendum bibendum. Cras interdum, ipsum id vulputate ornare, purus quam elementum ex, sit amet bibendum lacus nulla lobortis diam. Praesent imperdiet elementum est, a ornare orci ultricies in. Vestibulum id porttitor lacus, id imperdiet nibh. Vestibulum ante ipsum primis in faucibus orci luctus et ultrices posuere cubilia curae; Cras ligula dolor, consectetur ut consequat eu, pretium eget diam. Fusce a mollis quam.Nulla sagittis ut nibh non tincidunt. Etiam enim elit, consectetur at sapien et, placerat interdum enim. Praesent sodales vulputate purus quis consectetur. Nulla cursus augue vitae mi malesuada, ut molestie velit suscipit. Sed quis fermentum ante, et ultrices velit. Maecenas orci nunc, viverra sit amet ipsum quis, scelerisque tincidunt massa. Curabitur consectetur est turpis, nec rhoncus enim porta eu. Mauris porttitor faucibus bibendum.Nulla eleifend erat nunc. Donec sed neque ut elit convallis sodales ut ut neque. Fusce vehicula tempor est sodales interdum. Lorem ipsum dolor sit amet, consectetur adipiscing elit. Donec fermentum tempus eleifend. Sed nec nunc et purus finibus lobortis. Sed ac iaculis urna.Sed dictum velit sit amet ante congue convallis. Nulla facilisi. Sed suscipit, sapien at facilisis hendrerit, lacus arcu efficitur nisl, sed pellentesque metus tellus sit amet tortor. Vivamus vel libero placerat, elementum lacus ut, elementum velit. Donec finibus neque ante, at fringilla arcu ultricies ullamcorper. Nullam facilisis ut nunc nec maximus. Suspendisse sed ligula augue. Nullam posuere dolor at quam vestibulum, vel auctor ipsum fermentum. Donec efficitur sodales ex, eget convallis est accumsan vitae.Etiam in urna in neque molestie facilisis quis ac nisi. Duis vitae semper risus. Nulla molestie arcu non diam mollis, quis lobortis nibh malesuada. Vivamus feugiat id ipsum id efficitur. Maecenas sit amet mi purus. Donec et leo id tortor hendrerit euismod ac vel sem. Mauris ut mi eros. Sed lacinia molestie scelerisque. Donec ut erat at nisi maximus aliquet a in libero. Aliquam imperdiet sapien nunc, vitae dignissim ante dapibus sed. Ut mollis sit amet dui.";
    char* text1000 = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nullam tincidunt pretium urna, sit amet porta arcu. Curabitur mattis est sit amet sagittis ullamcorper. Ut nec nisl eu justo interdum varius. Fusce bibendum sapien eget consequat elementum. Pellentesque venenatis, massa et sodales fermentum, risus turpis luctus ipsum, eu varius neque nisi vitae nulla. Aliquam a condimentum mi. Morbi consequat pellentesque nisl eu luctus. Duis vehicula orci augue, a tincidunt ex imperdiet sed. Cras vitae maximus ex, eget vehicula turpis. Curabitur enim justo, tincidunt ac posuere eget, convallis non felis. Nam tellus felis, feugiat in fringilla posuere, aliquam quis justo. Vivamus id mollis nibh. Pellentesque finibus purus ex, vitae mattis mi gravida sit amet.In efficitur enim a rhoncus tempor. Cras vulputate maximus ante a vehicula. Nulla laoreet odio eget nulla lobortis consequat. Aenean quis convallis mauris. Mauris sed dui a elit dapibus dapibus eget sit amet turpis. Pellentesque orci aliquam.";
    char* texts[4] = {text1000, text3000, text5000, text7000};
    //create();
    int ret;
    int fd; 
    int i;
    char c; 
    char buffer[1024];
    char buffer2[8] = {50, 50, 50, 50, 50, 50, 50, 50};
    int size;
    int res;
    char vdiskname[200];

    printf ("started\n");

    /*if (argc != 2) {
	printf ("usage: app  <vdiskname>\n"); 
	exit(0); 
    }*/
    char* mode = argv[1];

    strcpy (vdiskname, "example");
    ret = vsfs_mount (vdiskname); 
    if (ret != 0) {
	printf ("could not mount \n");
	exit (1); 
    }

    printf ("creating files\n"); 
    for(int i = 0 ; i < 2 ; i++){
        printf("--------------\nFile %d:\n", i);
        char filename[10] = {'f', 'i', 'l', 'e', i + 65, '.', 'b', 'i', 'n', '\0'};
        res = vsfs_create(filename);
        if(res == -1){
            printf("Creation error!\n");
            break;
        }
        else{
            printf("\t%s was created!\n", filename);
            fd = vsfs_open(filename, MODE_APPEND);
            for(int j = 0 ; j < 4 ; j++){
                char* buf = texts[(i+j) % 4];
                printf("\tAppend byte: %lu\n", strlen(buf));
                res = vsfs_append(fd, (void*) buf, strlen(buf));
                printf("\n\nAfter append\n\n");
                if(res == -1){
                    printf("Append error\n");
                    vsfs_close(fd);
                    break;
                }
                else{
                    printf("\tSize is: %d\n", vsfs_getsize(fd));
                }
            }
            vsfs_close(fd);
            printf("Report:\n");
            printDisk();
        }
    }
    //vsfs_create ("file1.bin");
    //vsfs_create ("file2.bin");
    //vsfs_create ("file3.bin");
    
    //fd1 = vsfs_open ("file1.bin", MODE_APPEND);
    //fd2 = vsfs_open ("file2.bin", MODE_APPEND); 
    //printf("%d %d\n", fd1, fd2);

    /*for (i = 0; i < 10000; ++i) {
        buffer[0] =   (char) 65;  
        vsfs_append (fd1, (void *) buffer, 1);
    }
    //printf("Check1\n");
    for (i = 0; i < 10000; ++i) {
        buffer[0] = (char) 65;
        buffer[1] = (char) 66;
        buffer[2] = (char) 67;
        buffer[3] = (char) 68;
        //printf("Check2 %d\n", i);
        vsfs_append(fd2, (void *) buffer, 4);
        //printf("Check3\n");
    }*/

    
    /*vsfs_close(fd1); 
    vsfs_close(fd2); 

    fd = vsfs_open("file3.bin", MODE_APPEND);
    printf("Opened!\n");*/
    /*for (i = 0; i < 5; ++i) {
	memcpy (buffer, buffer2, 8); // just to show memcpy
	vsfs_append(fd, (void *) buffer, 8); 
        printf("We wrote:\n");
        for(int j = 0 ; j < 8 ; j++ ){
            printf("%s", ((char*)(buffer+j)));
        }
        printf("\n");
    }*/
    /*for(int i = 0 ; i < 20 ; i++){
        char num = (i + 50);
        printf("Check1111111\n");
        char ptr[8] = {num, 'r', 'a', 'n', 'd', 'o', 'm', '\0'};
        printf("Check222222\n");
        //ptr[0] = num;
        printf("Len: %lu\n", strlen(ptr));
        printf("%s\n", ptr);
        vsfs_append(fd, (void*) ptr, strlen(ptr));
        size = vsfs_getsize (fd);
        printf("Size of file3 is %d\n", size);
    }*/
    //vsfs_close (fd); 

    //fd = vsfs_open("file3.bin", MODE_READ);
    //size = vsfs_getsize (fd);
    //printf("Size of file3 is %d\n", size);
    //for (i = 0; i < size; ++i) {
	//vsfs_read (fd, (void *) buffer, 5);
    //for(int i = 0 ; i < 5 ; i++){
    //    printf("%s\n", ((char*)(buffer + i)));
    //}
	//c = (char) buffer[0];
	//c = c + 1;
    //}
    //vsfs_close (fd); 
    
    /*void* fileData = (void*) malloc(28);
    //fd = vsfs_open("file3.bin", MODE_READ);
    for(int i = 0 ; i < 17 ;i++)
    {
        vsfs_read(fd, fileData, 28);
        printf("Data is:\n");
        for(int i = 0 ; i < 28 ; i++){
            printf("%c", ((char*)(fileData + i))[0]);
        }
        printf("\n");
    }*/
    //}
    //vsfs_delete("file3.bin");
    //vsfs_close(fd);
    //vsfs_delete("file3.bin");
    //printf("checkpoint\n");
    //vsfs_open("file3.bin", MODE_APPEND);
    //printf("checkpoint\n");
    printDisk();
    ret = vsfs_umount();
    printf("checkpoint\n");
    //char a = (char)50;
    //printf("%s\n", &a);


    return 0;
}
