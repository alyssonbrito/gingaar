<?xml version="1.0" encoding="ISO-8859-1"?>
<ncl id="test_descriptor_size_14" xmlns="http://www.ncl.org.br/NCL3.0/EDTVProfile">
	<head>
		<regionBase>
			<region id="reg" width="720px" height="576px"/>
		</regionBase>
		<descriptorBase>
			<descriptor id="desc" region="reg" >
				<descriptorParam name="size" value="360,288" />
			</descriptor>
		</descriptorBase>

	</head>
	<body>
		<port id="port" component="image" />
		<media id="image" src="../../../../images/blue.jpg" descriptor="desc" />
	</body>
</ncl>
