var $MENU_TOGGLE = $('.menu-toggle');
var $NAV_BAR = $('.sidebar-navigation');
var $MENU_TOGGLE_ICON = $('.menu-toggle > i')
$MENU_TOGGLE.on("click", function(){
    $NAV_BAR.toggleClass("nav-min nav-full");
    $MENU_TOGGLE_ICON.toggleClass("fa-angle-right fa-angle-left")
})

$(".sidebar-navigation li > a").each(function() {
    //console.log($(this).attr('href'));
    if ((window.location.pathname.indexOf($(this).attr('href'))) > -1) {
        $(this).parent().addClass('active');
    }
});