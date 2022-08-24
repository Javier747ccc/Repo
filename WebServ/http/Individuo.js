function changePicture(smallPicture) {

			$("#big").attr("src", smallPicture.src);

    		$($("figcaption")[0]).html(smallPicture.alt);
    		$("figcaption:first").addClass("animateFigcaption");
    	}   	
	
    	$("#myDraggable").mouseover(function() {
    		$("figcaption:first").removeClass("animateFigcaption"); 
    	});    	

    	$("#myDraggable").draggable({
    		scroll: true,
    		axis: "x",
    		cursor: "move"    		
    	});     	

        $('.img-thumbnail').click(function() {
	        $(".selectable").find("img").removeClass('ui-selected');
	        $(this).addClass('ui-selected');
        }); 