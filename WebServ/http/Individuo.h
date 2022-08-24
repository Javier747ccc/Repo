<!DOCTYPE html>
<html lang="en">
<head>
  	<meta charset="utf-8">
  	<meta name="viewport" content="width=device-width, initial-scale=1, shrink-to-fit=no">
	<title>Responsive Photo Gallery</title>	
</head>
<body>
<link href="Individuo.css" rel="stylesheet" type="text/css">
<div class="page-header">
	<h1 class="text-center">Responsive Photo Gallery
		<br><small class="text-muted font-italic">Highlights of my travels</small>
		<hr>
	</h1>
</div>

<div class="container-fluid">	
	<div>
		<figure>
			<figcaption class="text-center font-italic">Everglades National Park</figcaption>
			<img
			id="big"
			src="https://scontent.fyyc6-1.fna.fbcdn.net/v/t1.0-9/14563560_1256198444432840_8098715639960638645_n.jpg?_nc_cat=103&_nc_oc=AQk4eS-Pju5sxwzD2oC2c4RgmJNGRyLXRejwJj3zlbWaSt1lhjuwqjoi6pCZHDnrg7Roz4cmgpFOKz8r7knktjVJ&_nc_ht=scontent.fyyc6-1.fna&oh=d4ebd2b2789ee87ed348e1c3c0511d28&oe=5DD7B198"
			alt="Everglades National Park"
			class="img-fluid d-block mx-auto">
		</figure>
	</div>

	<br>

	<div class="row-fluid selectable" id="myDraggable">
		<figure class="col-2 col-md-1 px-1">
			<img			
			src="https://scontent.fyyc6-1.fna.fbcdn.net/v/t1.0-9/14563560_1256198444432840_8098715639960638645_n.jpg?_nc_cat=103&_nc_oc=AQk4eS-Pju5sxwzD2oC2c4RgmJNGRyLXRejwJj3zlbWaSt1lhjuwqjoi6pCZHDnrg7Roz4cmgpFOKz8r7knktjVJ&_nc_ht=scontent.fyyc6-1.fna&oh=d4ebd2b2789ee87ed348e1c3c0511d28&oe=5DD7B198"		
			alt="Everglades National Park"	
			onclick="changePicture(this)"
			class="img-thumbnail">						
		</figure>

		<figure class="col-2 col-md-1 px-1">		 
			<img
			src="https://scontent.fyyc6-1.fna.fbcdn.net/v/t1.0-9/13177468_1143510949034924_7696033316372723784_n.jpg?_nc_cat=108&_nc_oc=AQkP3L7agXpy3t0at8PqNcfdwd4u9f3A-9JAsdnRrwTOO7k1I5_BVyReV1HrvcFK7jtrwiUPokmnWuXt8X78M625&_nc_ht=scontent.fyyc6-1.fna&oh=c6f0041509ae5789b8f74f280c7443c0&oe=5DDD535E"
			alt="Banff National Park"
			onclick="changePicture(this)"			
			class="img-thumbnail">			
		</figure>

		<figure class="col-2 col-md-1 px-1">		 
			<img
			src="https://scontent.fyyc6-1.fna.fbcdn.net/v/t1.0-9/13151386_1141144719271547_7538305944160687477_n.jpg?_nc_cat=106&_nc_oc=AQl7BCpK6IU55tVn8SngEsV1CCfhl36Hiy1oOrIJIwWLjonZgjj9R2snuuWEVU9iTu63VyQUEuKzNSpMtRR77wCS&_nc_ht=scontent.fyyc6-1.fna&oh=84b5997f08499b6b7818f092857309cf&oe=5DD80614"
			alt="Punta Cana"
			onclick="changePicture(this)"
			class="img-thumbnail">			
		</figure>

		<figure class="col-2 col-md-1 px-1">		 
			<img
			src="https://scontent.fyyc6-1.fna.fbcdn.net/v/t1.0-9/1175090_598403360212355_1188160761_n.jpg?_nc_cat=106&_nc_oc=AQl1mXX3vHjG7uvhP2cNXTNurxF38kgxN9hzfbJLZRcga6ucJ5zGoeSgI0w3ggfyfjiE_IVFZUWwxom7gFMvlTeE&_nc_ht=scontent.fyyc6-1.fna&oh=5ff9cd5f611c9edd7f5ee4719be2f591&oe=5DE83EC8"
			alt="Kennedy Space Center"
			onclick="changePicture(this)"
			class="img-thumbnail">			
		</figure>	

		<figure class="col-2 col-md-1 px-1">		 
			<img
			src="https://scontent.fyyc6-1.fna.fbcdn.net/v/t1.0-9/10702224_923028234416531_8268059090736625982_n.jpg?_nc_cat=107&_nc_oc=AQlRAEJ0SwgzvI8KW4a83sLFkevgIyu0uMfh5sozm2fi-DTlwhrjiZYYj9WhBp_gt4um3nslwcp3dKl5XI3hYAC2&_nc_ht=scontent.fyyc6-1.fna&oh=8733aee8d3e68e7b7919a84f8eab05f8&oe=5DE8B28C"
			alt="Bahamas"
			onclick="changePicture(this)"
			class="img-thumbnail">			
		</figure>

		<figure class="col-2 col-md-1 px-1">		 
			<img
			src="https://scontent.fyyc6-1.fna.fbcdn.net/v/t1.0-9/10151424_811624942223528_6279368280953636501_n.jpg?_nc_cat=103&_nc_oc=AQkb3bmXiwrjdI6tWNtBf_N4jhIuG4PU36u6VteeYrvV6MFXhcAitA3nbvToEATZEXUOVcwTLlvvS9vX4CCBctAi&_nc_ht=scontent.fyyc6-1.fna&oh=0f69bf3d4e494f6db84c2b1f7920b59a&oe=5DE4D4FA"
			alt="Cancun"
			onclick="changePicture(this)"
			class="img-thumbnail">			
		</figure>

		<figure class="col-2 col-md-1 px-1">		 
			<img
			src="https://scontent.fyyc6-1.fna.fbcdn.net/v/t1.0-9/10698435_800372960015393_7768646737427525057_n.jpg?_nc_cat=108&_nc_oc=AQkZiZQT1A0NPa1XmkrYwumOUuoBhayaspVnwoNQryuAbxeC_LjePx4595TkQT6wqzeA8MBAebsYDG7phr80nloD&_nc_ht=scontent.fyyc6-1.fna&oh=59f0e73debadb06e3da0eeb6376c538b&oe=5DE0B3F4"
			alt="US Virgin Islands"
			onclick="changePicture(this)"
			class="img-thumbnail">			
		</figure>

		<figure class="col-2 col-md-1 px-1">		 
			<img
			src="https://scontent.fyyc6-1.fna.fbcdn.net/v/t1.0-9/546901_457113374341355_1519228069_n.jpg?_nc_cat=106&_nc_oc=AQm9gMVdqAEwhXNOv1BHrFkKxLKRxnMGZ5UM3i3qlPgdGrYmkT0RvN3jU0KQor3q6gr39fad0PjItsfQeUYhW9W7&_nc_ht=scontent.fyyc6-1.fna&oh=739e4f7e4d92b370562f6aa10a57e6df&oe=5DE9EFFE"
			alt="Vancouver"
			onclick="changePicture(this)"
			class="img-thumbnail">	
		</figure>

		<figure class="col-2 col-md-1 px-1">		 
			<img
			src="https://scontent.fyyc6-1.fna.fbcdn.net/v/t1.0-9/539386_452313374821355_579085132_n.jpg?_nc_cat=103&_nc_oc=AQlf_JqgK747hXsppONAIGvK3SwghlhLdP20yrA-OFUGKOx3ZRIzZkQIOnFNm_Et_OOnNK8cbtcALhTSRHDP_1kz&_nc_ht=scontent.fyyc6-1.fna&oh=f8cfc1173c6bf20ebbe3249a1d30e9e8&oe=5DE3E9E7"
			alt="Honolulu"
			onclick="changePicture(this)"
			class="img-thumbnail">	
		</figure>

		<figure class="col-2 col-md-1 px-1">		 
			<img
			src="https://scontent.fyyc6-1.fna.fbcdn.net/v/t1.0-9/309245_239648696087825_4564427_n.jpg?_nc_cat=100&_nc_oc=AQkzjSiClIDYYk5v32KVc87qEdcB8gMlgwDfazHwBdqJjdWTtdGpm_aHY3ZU0_NSo1dwobyfrajU1At5NGirL_f0&_nc_ht=scontent.fyyc6-1.fna&oh=4a5c513e93022cb8d34d322dbdb3d295&oe=5DDB2327"
			alt="Hollywood"
			onclick="changePicture(this)"
			class="img-thumbnail">	
		</figure>

		<figure class="col-2 col-md-1 px-1">		 
			<img
			src="https://scontent.fyyc6-1.fna.fbcdn.net/v/t1.0-9/298622_238905802828781_1381900701_n.jpg?_nc_cat=102&_nc_oc=AQnmx7XCxZmdtxU3VTr__cotlPyic_43XrujNTbP5TjDSxxUbCJ8TsrKdA_IuQJVZOt7GtTnyHl1eQ6uEQegeLvX&_nc_ht=scontent.fyyc6-1.fna&oh=0eb346d13b3f01f8099e7575d523317b&oe=5DCD5657"
			alt="Grand Canyon"
			onclick="changePicture(this)"
			class="img-thumbnail">	
		</figure>

		<figure class="col-2 col-md-1 px-1">		 
			<img
			src="https://scontent.fyyc6-1.fna.fbcdn.net/v/t1.0-9/321003_236261479759880_958097213_n.jpg?_nc_cat=111&_nc_oc=AQmhRGlMsMr-sfNO_ZTiIBLbOMhgzVXxklwSFAFwJmaJMRs4Hd7zMgLjkI9qYUoe9pqAC72Qrmd-JKKJfI0BRwIr&_nc_ht=scontent.fyyc6-1.fna&oh=31fadacf517b23aa9db6c2b99a2869d1&oe=5DCDA329"
			alt="Las Vegas"
			onclick="changePicture(this)"
			class="img-thumbnail">	
		</figure>

		<figure class="col-2 col-md-1 px-1">		 
			<img
			src="https://scontent.fyyc6-1.fna.fbcdn.net/v/t1.0-9/308474_247255665327128_1941920554_n.jpg?_nc_cat=102&_nc_oc=AQmWzTKY3Ul_lW0jnN7EhfO_fBIlpFkTJd8NAXeZOzQb_ZPA5c7s4mfF78wledpweB1uN_1oT7-za2KLF_90mFMq&_nc_ht=scontent.fyyc6-1.fna&oh=2eb09ae186f597fde868b6975fdb744e&oe=5DD14F8C"
			alt="Universal Studios Hollywood"
			onclick="changePicture(this)"
			class="img-thumbnail">	
		</figure>
	</div>
</div>

<footer class="text-center">&copy; 2017 - Site by <a href="http://stackoverflow.com/users/2826858/thomas-soos?tab=profile" target="_blank">Thomas Soos</a></footer>
<script src="Individuo.js"></script>
</body>
</html>