<?php
//	require_once('simplepie.inc');
	require_once('php/autoloader.php');
	// CHANGE THE FEED ADDRESS BELOW - THAT'S IT!
	$feed = new SimplePie('http://www.imemc.org/rssfullposts.xml');
	
	$feed->handle_content_type();
	
	$total_articles = 3;
	
	for ($x = 0; $x < $feed->get_item_quantity($total_articles); $x++)
	{
		$first_items[] = $feed->get_item($x);
	}
?>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">

<head>
	<meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
	<meta name="viewport" content="width=320" />

	<title>iPhone Interface by CSS-Tricks</title>
	
	<link rel="stylesheet" type="text/css" href="mobilestyle.css" />
</head>

<body>

	<div id="page-wrap">
	</div>

</body>