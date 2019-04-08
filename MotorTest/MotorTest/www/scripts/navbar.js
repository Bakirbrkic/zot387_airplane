//alert("selam");
$(".appNav").on("click", function () {
    $(".sideBarWrap").removeClass("hideDarknes");
    $(".sideBar").removeClass("hideSideBar");

    $(".sideBarWrap").addClass("showDarknes");
    $(".sideBar").addClass("showSideBar");
});

function hideSideBar() {
    $(".sideBarWrap").addClass("hideDarknes");
    $(".sideBar").addClass("hideSideBar");
    setTimeout(function () {
        $(".sideBarWrap").removeClass("showDarknes");
        $(".sideBar").removeClass("showSideBar");
    }, 500);

}

$(".closeSideBar").on("click", function () {
    hideSideBar();
});
$(".sideBarWrap").click(function () {
    hideSideBar();
}).children().click(function (e) {
    return false;
});

$(".moreOpt").click(function () {
    var ipAdd = prompt("What is the airplanes IP address?");
    localStorage.setItem("ipAdd", ipAdd);
})
////////////////////////////////////////////////////////////////////
