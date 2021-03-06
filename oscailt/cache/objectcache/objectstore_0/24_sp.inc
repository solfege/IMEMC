<?php global $pseudo_directory_offset; $site_file_store = $pseudo_directory_offset."attachments/sites/default/"; 
$oscailt_page_setting = Array("site_module_settings" => Array("siteobject" => "1", "newswireobject" => "2126", "articleobject" => "53", "featureobject" => "54", "galleryobject" => "64", "searchobject" => "2212", "archiveobject" => "55", "contactobject" => "84", "commentsobject" => "", "publishobject" => "57", "eventsobject" => "58"), "site_mail_settings" => Array("to" => "news@imemc.org", "from" => "site@imemc.org", "subject_prefix" => "Article edit:", "replyto" => "news@imemc.org", "level" => "none"), "site_filters" => Array("topic" => Array("0" => "*"), "region" => Array("0" => "20", "1" => "19", "2" => "18", "3" => "17", "4" => "16", "5" => "15", "6" => "14", "7" => "13", "8" => "12", "9" => "11", "10" => "10", "11" => "9", "12" => "8", "13" => "7", "14" => "6", "15" => "5", "16" => "4", "17" => "3", "18" => "2", "19" => "1"), "language" => Array("0" => "en"), "type" => Array("0" => "*"), "author" => ""), "obj_id" => "24", "obj_type" => "DOCUMENT", "site_id" => "1");
$oscailt_story_query = new StoryQuery();
$oscailt_story_query->loadFormValues();
$PAGE_LAYOUT_ID = 2032; ?>
<?
         addToPageDescription("The IMEMC is a media collective. We are independent journalists producing and distributing the authentic voices of the people.  We are located in Beit Sahour, in the Occupied West Bank, and we welcome new volunteers and contributors.");
      addToPageTitle("Qui�?©nes Somos");
         addToPageDescription("Sobre El Centro Internacional de Medios de Comunicaci�?�?�?³n de Oriente Medio"); ?>

<!-- <Qui�?©nes somos obj id 24> -->
<h1 class='indypagebody'>Sobre el IMEMC</h1><BLOCKQUOTE class='indypagebody'>El IMEMC es un centro de informaci�?³n creado en colaboraci�?³n entre palestinos y periodistas extranjeros con el objetivo de ofrecer covertura medi�?¡tica independiente en ingl�?©s, y ahora en espa�?±ol, acerca de Israel-Palestina.</BLOCKQUOTE><DIV class='indypagebody'>El hecho de ser una iniciativa com�?ºn entre palestinos y extranjeros, aporta al IMEMC un profundo conocimiento del contexto, la historia y el entorno sociopol�?­tico por parte de los periodistas palestinos, combin�?¡ndolo con la objetividad e imparcialidad de los periodistas extranjeros.br /><br />

El IMEMC ofrece un extensa y equilibrada covertura de los hechos y su desarrollo en Israel-Palestina..<br /><br />

El IMEMC es miembro fundador de la red United Radio y TV Stations (NUR Media) english.nurmedia.org<br /><br />

El IMEMC ofrece covertura medi�?¡tica de noticias, acontecimientos pol�?­ticos o hechos puntuales adem�?¡s de reportages, art�?­culos de an�?¡lisis pol�?­tico y una selecci�?³n de art�?­culos de opini�?³n. <br /><br />

El IMEMC produce semanalmente un resumen en audio del los acontecimientos sociopol�?­ticos en Israel y Palestina para mantenerte al d�?­a.<br /><br />

<b>Lo que necesitas saber para aprovechar al maximo nuestra p�?¡gina web de not�?­cias IMEMC:</b><br /><br />

En el centro de la p�?¡gina encontrar�?¡s los art�?­culos de not�?­cias sociopol�?­ticas m�?¡s recientes.<br /><br />

La secci�?³n "�?šltima hora" ofrece una covertura actualizada cada hora. La lista pretende mantener informados a los lectores acerca de los acontecimientos que tengan lugar en Cisjordania y en la franja de Gaza. La informaci�?³n la aportan los colaboradores y miembros del IMEMC que se encuentran sobre el terreno.<br /><br />

La secci�?³n "Inter�?©s Humano" presenta historias de enfoque m�?¡s personal acerca del conflicto.<br /><br />

Los art�?­culos que se incluyen en la secci�?³n "Opini�?³n y An�?¡lisis" pueden ser aportaciones hechas a nuestra p�?¡gina por cualquier autor que pueda contribuir, tanto desde el interior como desde fuera de Palestina. �?¡Incluso t�?º mismo!<br /><br />

Cuando abras un art�?­culo, una lista de art�?­culos relacionados aparecer�?¡ en un recuadro lateral. Esta lista ofrece al lector la posibilidad f�?¡cil y c�?³moda de profundizar en un tema concreto y de contextualizar a trav�?©s de otros acontecimientos relacionados. Tambi�?©n puedes utilizar este tipo de filtros para ver todos los art�?­culos de un tipo determinado, �?¡rea o tema de particular inter�?©s para t�?­.<br /><br /></DIV>
<!-- </Qui�?©nes somos obj id 24> -->

<?php getPageLanguageOptions("ot", Array("0" => "en", "1" => "sp"), "24"); ?>
         <?php
          global $editor_session, $pseudo_directory_offset;
          if(($editor_session->canEditLayout()|| $editor_session->canEditContent()) && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobjdocument", 1)))
          {
          ?>
             <br>
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=document&amp;target_id=24&amp;action=edit&amp;site_id=1&amp;obj_language=ot&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0  align="left" title="Edit Static Document [Qui�?©nes somos] Configuration" alt="Edit Static Document [Qui�?©nes somos] Configuration" src="<?php echo $pseudo_directory_offset?>graphics/mediaicons/32x32/editorial2.png" /></a>
             
          <?
          }
          ?>
          