
var questions = [
  
 {question:"Cual es tu nombre?"},
  {question:"What's your last name?"},
  {question:"Radio1?", type: "radio",answer1:"seleccion1",answer2:"seleccion2",answer3:"seleccion3",answer4:"seleccion4"},
  {question:"Radio2?", type: "radio",answer1:"Sel1",answer2:"Sel2",answer3:"Sel3"},
  {question:"What's your email?", pattern: /^[^\s@]+@[^\s@]+\.[^\s@]+$/},
  {question:"Create your password", type: "password"}
]


function post(path, params, method) {
    method = method || "post"; // Set method to post by default if not specified.

    // The rest of this code assumes you are not using a library.
    // It can be made less wordy if you use one.
    var form = document.createElement("form");
    form.setAttribute("method", method);
    form.setAttribute("action", path);

    for(var key in params) {
        if(params.hasOwnProperty(key)) {
            var hiddenField = document.createElement("input");
            hiddenField.setAttribute("type", "hidden");
            hiddenField.setAttribute("name", key);
            hiddenField.setAttribute("value", params[key]);

            form.appendChild(hiddenField);
        }
    }

    document.body.appendChild(form);
    form.submit();
}
/*
  do something after the questions have been answered
*/
var onComplete = function() 
{
    var h1 = document.createElement('h1')
    //h1.appendChild(document.createTextNode('Thanks ' + questions[0].answer + ' for checking this pen out!'))
	h1.appendChild(document.createTextNode(questions[0].answer + ',' + questions[1].answer + ',' + questions[2].answer + ',' + questions[3].answer + ',' + questions[4].answer + ',' + questions[5].answer ))
	
	post('/contact/', {name: 'Johnny Bravo'});
	
    setTimeout(function() 
	{
      register.parentElement.appendChild(h1)
      setTimeout(function() { h1.style.opacity = 1 }, 50)
    }, 1000)
}

;(function(questions, onComplete) 
{
    var tTime = 100 // transition transform time from #register in ms
    var wTime = 200 // transition width time from #register in ms
    var eTime = 1000 // transition width time from inputLabel in ms
	
	
	
	
    // init
    // --------------
    if (questions.length == 0) return

    var position = 0

    putQuestion()

    forwardButton.addEventListener('click', validate)
    button1.addEventListener("click", validate);
	
	inputField.addEventListener('keyup', function(e) 
	{
        transform(0, 0) // ie hack to redraw
        if (e.keyCode == 13) validate()
    })

    previousButton.addEventListener('click', function(e) 
	{
        if (position === 0) return
        position -= 1
        hideCurrent(putQuestion)
    })


    // functions
    // --------------

    // load the next question
    function putQuestion() 
	{
		
		
		if (questions[position].type=="radio")
		{
			register0.style.display= "none";
			Tradio.style.display = "block";
			
			label1.innerHTML = questions[position].answer1;
			label2.innerHTML = questions[position].answer2;
			label3.innerHTML = questions[position].answer3;
			label4.innerHTML = questions[position].answer4;
			
			radio1.checked = false;
			radio2.checked = false;
			radio3.checked = false;
			radio4.checked = false;
			
			
			
			
			inputField.value = '';
			inputField.focus()
			previousButton.className = position ? 'ion-android-arrow-back' : 'ion-person'
			return;
	
		}
		else
		{
			register0.style.display= "block";
			Tradio.style.display= "none";
			
			
			inputLabel.innerHTML = questions[position].question
			inputField.type = questions[position].type || 'text'
			// inputField.value = questions[position].answer || ''
			inputField.value = '';
			inputField.focus()
		}
        // set the progress of the background
        
        previousButton.className = position ? 'ion-android-arrow-back' : 'ion-person'

        showCurrent()

    }

    // when submitting the current question
    function validate() 
	{
		
		if (questions[position].type=="radio")
		{
			var nSel = 0;
			if (radio1.checked)
				nSel = 1;
			if (radio2.checked)
				nSel = 2;
			if (radio3.checked)
				nSel = 3;
			if (radio4.checked)
				nSel = 4;
			
			
			
			if(nSel>0)
			{
				questions[position].answer = nSel;
				
				++position
				if (questions[position]) hideCurrent(putQuestion)
				else hideCurrent(function() 
				{
					// remove the box if there is no next question
					register.className = 'close'
					onComplete()
				})
			}
			else
			{
				label5.innerHTML = "Seleccione una de las opciones";
				
			}
			
			
		}
		else
		{
			var validateCore = function() 
			{      
			  return inputField.value.match(questions[position].pattern || /.+/)
			}

			if (!questions[position].validate) 
			{
				questions[position].validate = validateCore
			}

			// check if the pattern matches
			if (!questions[position].validate()) 
			{
				wrong(inputField.focus.bind(inputField))
			}
			else 
				ok(function() 
			{
				
				// execute the custom end function or the default value set
				if (questions[position].done) questions[position].done()
				else questions[position].answer = inputField.value

				++position

				// if there is a new question, hide current and load next
				if (questions[position]) hideCurrent(putQuestion)
				else hideCurrent(function() 
				{
					// remove the box if there is no next question
					register.className = 'close'
					onComplete()
				})
			})
		}
    }


    // helper
    // --------------

    function hideCurrent(callback) 
	{
        inputContainer.style.opacity = 0
        inputLabel.style.marginLeft = 0
        inputProgress.style.width = 0
        inputProgress.style.transition = 'none'
        inputContainer.style.border = null
        setTimeout(callback, wTime)
    }

    function showCurrent(callback) 
	{
        inputContainer.style.opacity = 1
        inputProgress.style.transition = ''
        inputProgress.style.width = '100%'
        setTimeout(callback, wTime)
    }

    function transform(x, y) 
	{
        register.style.transform = 'translate(' + x + 'px ,  ' + y + 'px)'
    }

    function ok(callback) 
	{
        register.className = ''
        setTimeout(transform, tTime * 0, 0, 10)
        setTimeout(transform, tTime * 1, 0, 0)
        setTimeout(callback, tTime * 2)
    }

    function wrong(callback) 
	{
        register.className = 'wrong'
        for (var i = 0; i < 6; i++) // shaking motion
            setTimeout(transform, tTime * i, (i % 2 * 2 - 1) * 20, 0)
        setTimeout(transform, tTime * 6, 0, 0)
        setTimeout(callback, tTime * 7)
    }

}(questions, onComplete))



