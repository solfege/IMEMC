<?php global $pseudo_directory_offset; $site_file_store = $pseudo_directory_offset."attachments/sites/default/"; 
$oscailt_page_setting = Array("site_module_settings" => Array("siteobject" => "1", "newswireobject" => "2126", "articleobject" => "53", "featureobject" => "54", "galleryobject" => "64", "searchobject" => "2212", "archiveobject" => "55", "contactobject" => "84", "commentsobject" => "", "publishobject" => "57", "eventsobject" => "58"), "site_mail_settings" => Array("to" => "news@imemc.org", "from" => "site@imemc.org", "subject_prefix" => "Article edit:", "replyto" => "news@imemc.org", "level" => "none"), "site_filters" => Array("topic" => Array("0" => "*"), "region" => Array("0" => "20", "1" => "19", "2" => "18", "3" => "17", "4" => "16", "5" => "15", "6" => "14", "7" => "13", "8" => "12", "9" => "11", "10" => "10", "11" => "9", "12" => "8", "13" => "7", "14" => "6", "15" => "5", "16" => "4", "17" => "3", "18" => "2", "19" => "1"), "language" => Array("0" => "en"), "type" => Array("0" => "*"), "author" => ""), "obj_id" => "2100", "obj_type" => "LIST", "site_id" => "1");
$oscailt_story_query = new StoryQuery();
$oscailt_story_query->loadFormValues();
$PAGE_LAYOUT_ID = 2032; ?>
<?
         addToPageDescription("The IMEMC is a media collective. We are independent journalists producing and distributing the authentic voices of the people.  We are located in Beit Sahour, in the Occupied West Bank, and we welcome new volunteers and contributors.");
      addToPageTitle("Feeds");
         addToPageDescription("A list of the feeds provide by the site"); ?>

<!-- <Feeds obj id 2100> -->
<h1 class='indypagebody'>Feeds</h1>As well as viewing IMEMC News content directly from the website, you can also access it using what are known as "feeds". These are a structured form of the site's data which can be read by many other pieces of software, including desktop news readers which can aggregate all your favourite news sources and notify you when new news is available. 
<br><br> 
There are two feed technologies currently in use and they are known as RSS and Atom, both of which are supported by Indymedia Palestine. RSS is the more popular feed format, though Atom is more standardised and has some advantages over it. 
<br><br> 
To access our feeds just click on the one of the feed buttons on the left sidebar and if you have a feed reader installed it should automatically open up and subscribe you to the feed. 
<br><br> 
We have several feed types listed below which offer different forms of our content: 

<H2>Summaries Feed <img src="attachments/sites/site_1/rsssummariesbutton.png"> <img src="attachments/sites/site_1/atomsummariesbutton.png"></H2> 
This is the default feed with just article titles and summaries. Its ideal if your using the feed to get notifications and then follow the link to the website. 

<H2>Full Posts Feed <img src="attachments/sites/site_1/rssfullpostsbutton.png"> <img src="attachments/sites/site_1/atomfullpostsbutton.png"></H2> 
This feed contains the full article contents and is ideal if you wish to view the entire article in your news reader application. Please note however that if an article has attachments then RSS will only display the first one, while Atom feeds will display them all. This is a limitation of RSS. 

<H2>Comment Feeds <img src="attachments/sites/site_1/rsscommentsbutton.png"> <img src="attachments/sites/site_1/atomcommentsbutton.png"></H2> 
You can also track comments posted to a particular article by monitoring a feed of that articles comments. To access these comment feeds just look at the bottom of each article where you will find an RSS and Atom button linking to the feed. 
<br> <br> 
<img align=right src="attachments/sites/site_1/itunesimage.jpg" alt="iTunes Music Store Podcast Logo" title="iTunes Music Store Podcast Logo" width=150 height=150> 
<H2>Podcast Feed <img src="attachments/sites/site_1/rsspodcastbutton.png"> <img src="attachments/sites/site_1/itunesbutton.gif"></H2> 
We also provide a Podcast feed of all our audio and video content, which can be viewed in iTunes or any other podcast client. Please note that only audio and video content which is quicktime or mpeg compliant will be included in this feed. 
<br><br> 
The iTunes button will take you to the Indymedia Ireland entry on the iTunes Music Store where you can subscribe to our feed. The podcast button will open the podcast in your installed newsreader (which will need to support enclosures if you wish to see the audio and video content). 
<br clear=right> 
<H2>Customised Feeds</H2> 
While our feeds default to showing all content you can customize a feed to show only a subset of content based on certain criteria. These criteria include 
<ul> 
<li>author_name 
<li>search_text 
<li>topic 
<li>region 
<li>type 
</ul> 

You can use these criteria by appending them to the end of the feed url as follows 

<ul> 
<li>For the topic Wall append <i>?topic=wall</i> 
<li>For the region Nablus append <i>?region=nablus</i> 
<li>For articles of type feature append <i>?type=feature</i> 
<li>For articles where the word apartheid occurs append <i>?search_text=apartheid</i> 
<li>For articles by the author redjade append <i>?author_name=redjade</i> 
</ul> 

You can combine these values also by using an ampersand so if you want to get a feed of articles about settlement in the Hebron area append <i>?region=hebron&topic=settlement</i> 
<br><br> 
If the topic, type or region you wish to search for has spaces or hyphens you can leave them out, so <i>Gaza Strip</i> would become <i>gazastrip</i>.
<br><br> 
For spaces in the search_text replace them with %20, so <i>?search_text=mahmoud abbas</i> would become <i>?search_text=mahmoud%20abbas</i> 
<br><br> 
Note that all text is case insensitive. 
<BR><BR> 
If you have any queries about our feeds please let us know using the contact form. 
<BR><BR> 
<BR><BR><UL class='indypagebody'>
<LI class='indypagebody'>
         <?php
          global $editor_session, $graphics_store;
          if($editor_session->canEditLayout() && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobjlink", 1)))
          {
          ?>
             
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=link&amp;target_id=2103&amp;action=edit&amp;site_id=1&amp;obj_language=en&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0 title="Edit Simple Link [Atom Summaries Link] Configuration" alt="Edit Simple Link [Atom Summaries Link] Configuration" src="<?php echo $graphics_store?>editobject.gif" /></a>
             
          <?
          }
          ?>
       

<!-- <Atom Summaries Link obj id 2103> -->
<a href='http://www.imemc.org/atom.xml' class='indypagebody' title='An atom feed of article summaries'>Atom Summaries</a>
<!-- </Atom Summaries Link obj id 2103> -->

</LI>
<LI class='indypagebody'>
         <?php
          global $editor_session, $graphics_store;
          if($editor_session->canEditLayout() && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobjlink", 1)))
          {
          ?>
             
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=link&amp;target_id=2104&amp;action=edit&amp;site_id=1&amp;obj_language=en&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0 title="Edit Simple Link [Atom Full Posts Link] Configuration" alt="Edit Simple Link [Atom Full Posts Link] Configuration" src="<?php echo $graphics_store?>editobject.gif" /></a>
             
          <?
          }
          ?>
       

<!-- <Atom Full Posts Link obj id 2104> -->
<a href='<? echo $pseudo_directory_offset; ?>atomfullposts.xml' class='indypagebody' title='An Atom Feed of articles with full content'>Atom Full Posts</a>
<!-- </Atom Full Posts Link obj id 2104> -->

</LI>
<LI class='indypagebody'>
         <?php
          global $editor_session, $graphics_store;
          if($editor_session->canEditLayout() && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobjlink", 1)))
          {
          ?>
             
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=link&amp;target_id=2101&amp;action=edit&amp;site_id=1&amp;obj_language=en&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0 title="Edit Simple Link [RSS Summaries Link] Configuration" alt="Edit Simple Link [RSS Summaries Link] Configuration" src="<?php echo $graphics_store?>editobject.gif" /></a>
             
          <?
          }
          ?>
       

<!-- <RSS Summaries Link obj id 2101> -->
<a href='<? echo $pseudo_directory_offset; ?>rss.xml' class='indypagebody' title='An RSS feed of article summaries'>RSS Summaries</a>
<!-- </RSS Summaries Link obj id 2101> -->

</LI>
<LI class='indypagebody'>
         <?php
          global $editor_session, $graphics_store;
          if($editor_session->canEditLayout() && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobjlink", 1)))
          {
          ?>
             
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=link&amp;target_id=2102&amp;action=edit&amp;site_id=1&amp;obj_language=en&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0 title="Edit Simple Link [RSS Full Posts Link] Configuration" alt="Edit Simple Link [RSS Full Posts Link] Configuration" src="<?php echo $graphics_store?>editobject.gif" /></a>
             
          <?
          }
          ?>
       

<!-- <RSS Full Posts Link obj id 2102> -->
<a href='<? echo $pseudo_directory_offset; ?>rssfullposts.xml' class='indypagebody' title='A feed articles containing their full content'>RSS Full Posts</a>
<!-- </RSS Full Posts Link obj id 2102> -->

</LI>
<LI class='indypagebody'>
         <?php
          global $editor_session, $graphics_store;
          if($editor_session->canEditLayout() && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobjlink", 1)))
          {
          ?>
             
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=link&amp;target_id=2105&amp;action=edit&amp;site_id=1&amp;obj_language=en&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0 title="Edit Simple Link [Podcast Link] Configuration" alt="Edit Simple Link [Podcast Link] Configuration" src="<?php echo $graphics_store?>editobject.gif" /></a>
             
          <?
          }
          ?>
       

<!-- <Podcast Link obj id 2105> -->
<a href='<? echo $pseudo_directory_offset; ?>podcast.xml' class='indypagebody' title='A podcast of the sites audio and video content'>Podcast</a>
<!-- </Podcast Link obj id 2105> -->

</LI>
<LI class='indypagebody'>
         <?php
          global $editor_session, $graphics_store;
          if($editor_session->canEditLayout() && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobjlink", 1)))
          {
          ?>
             
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=link&amp;target_id=2106&amp;action=edit&amp;site_id=1&amp;obj_language=en&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0 title="Edit Simple Link [iTunes Store Podcast Link] Configuration" alt="Edit Simple Link [iTunes Store Podcast Link] Configuration" src="<?php echo $graphics_store?>editobject.gif" /></a>
             
          <?
          }
          ?>
       

<!-- <iTunes Store Podcast Link obj id 2106> -->
<a href='itms://phobos.apple.com/WebObjects/MZStore.woa/wa/viewPodcast?id=120904798' class='indypagebody' title='A link to the sites podcast in the iTunes music store'>iTunes Podcast</a>
<!-- </iTunes Store Podcast Link obj id 2106> -->

</LI>
</UL>

<!-- </Feeds obj id 2100> -->


         <?php
          global $editor_session, $pseudo_directory_offset;
          if($editor_session->canEditLayout() && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobjlist", 1)))
          {
          ?>
             <br>
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=list&amp;target_id=2100&amp;action=edit&amp;site_id=1&amp;obj_language=en&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0  align="left" title="Edit Listing Box [Feeds] Configuration" alt="Edit Listing Box [Feeds] Configuration" src="<?php echo $pseudo_directory_offset?>graphics/mediaicons/32x32/listingbox.png" /></a>
             
          <?
          }
          ?>
          