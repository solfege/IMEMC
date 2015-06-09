<?php global $pseudo_directory_offset; $site_file_store = $pseudo_directory_offset."attachments/sites/default/"; 
$oscailt_page_setting = Array("site_module_settings" => Array("siteobject" => "1", "newswireobject" => "2126", "articleobject" => "53", "featureobject" => "54", "galleryobject" => "64", "searchobject" => "2212", "archiveobject" => "55", "contactobject" => "84", "commentsobject" => "", "publishobject" => "57", "eventsobject" => "58"), "site_mail_settings" => Array("to" => "news@imemc.org", "from" => "site@imemc.org", "subject_prefix" => "Article edit:", "replyto" => "news@imemc.org", "level" => "none"), "site_filters" => Array("topic" => Array("0" => "*"), "region" => Array("0" => "20", "1" => "19", "2" => "18", "3" => "17", "4" => "16", "5" => "15", "6" => "14", "7" => "13", "8" => "12", "9" => "11", "10" => "10", "11" => "9", "12" => "8", "13" => "7", "14" => "6", "15" => "5", "16" => "4", "17" => "3", "18" => "2", "19" => "1"), "language" => Array("0" => "en"), "type" => Array("0" => "*"), "author" => ""), "obj_id" => "24", "obj_type" => "DOCUMENT", "site_id" => "1");
$oscailt_story_query = new StoryQuery();
$oscailt_story_query->loadFormValues();
$PAGE_LAYOUT_ID = 2032; ?>
<?
         addToPageDescription("The IMEMC is a media collective. We are independent journalists producing and distributing the authentic voices of the people.  We are located in Beit Sahour, in the Occupied West Bank, and we welcome new volunteers and contributors.");
      addToPageTitle("QuiÃ?Â©nes Somos");
         addToPageDescription("Sobre El Centro Internacional de Medios de ComunicaciÃ?Æ?Ã?Â³n de Oriente Medio"); ?>

<!-- <QuiÃ?Â©nes somos obj id 24> -->
<h1 class='indypagebody'>Sobre el IMEMC</h1><BLOCKQUOTE class='indypagebody'>El IMEMC es un centro de informaciÃ?Â³n creado en colaboraciÃ?Â³n entre palestinos y periodistas extranjeros con el objetivo de ofrecer covertura mediÃ?Â¡tica independiente en inglÃ?Â©s, y ahora en espaÃ?Â±ol, acerca de Israel-Palestina.</BLOCKQUOTE><DIV class='indypagebody'>El hecho de ser una iniciativa comÃ?Âºn entre palestinos y extranjeros, aporta al IMEMC un profundo conocimiento del contexto, la historia y el entorno sociopolÃ?Â­tico por parte de los periodistas palestinos, combinÃ?Â¡ndolo con la objetividad e imparcialidad de los periodistas extranjeros.br /><br />

El IMEMC ofrece un extensa y equilibrada covertura de los hechos y su desarrollo en Israel-Palestina..<br /><br />

El IMEMC es miembro fundador de la red United Radio y TV Stations (NUR Media) english.nurmedia.org<br /><br />

El IMEMC ofrece covertura mediÃ?Â¡tica de noticias, acontecimientos polÃ?Â­ticos o hechos puntuales ademÃ?Â¡s de reportages, artÃ?Â­culos de anÃ?Â¡lisis polÃ?Â­tico y una selecciÃ?Â³n de artÃ?Â­culos de opiniÃ?Â³n. <br /><br />

El IMEMC produce semanalmente un resumen en audio del los acontecimientos sociopolÃ?Â­ticos en Israel y Palestina para mantenerte al dÃ?Â­a.<br /><br />

<b>Lo que necesitas saber para aprovechar al maximo nuestra pÃ?Â¡gina web de notÃ?Â­cias IMEMC:</b><br /><br />

En el centro de la pÃ?Â¡gina encontrarÃ?Â¡s los artÃ?Â­culos de notÃ?Â­cias sociopolÃ?Â­ticas mÃ?Â¡s recientes.<br /><br />

La secciÃ?Â³n "Ã?Å¡ltima hora" ofrece una covertura actualizada cada hora. La lista pretende mantener informados a los lectores acerca de los acontecimientos que tengan lugar en Cisjordania y en la franja de Gaza. La informaciÃ?Â³n la aportan los colaboradores y miembros del IMEMC que se encuentran sobre el terreno.<br /><br />

La secciÃ?Â³n "InterÃ?Â©s Humano" presenta historias de enfoque mÃ?Â¡s personal acerca del conflicto.<br /><br />

Los artÃ?Â­culos que se incluyen en la secciÃ?Â³n "OpiniÃ?Â³n y AnÃ?Â¡lisis" pueden ser aportaciones hechas a nuestra pÃ?Â¡gina por cualquier autor que pueda contribuir, tanto desde el interior como desde fuera de Palestina. Ã?Â¡Incluso tÃ?Âº mismo!<br /><br />

Cuando abras un artÃ?Â­culo, una lista de artÃ?Â­culos relacionados aparecerÃ?Â¡ en un recuadro lateral. Esta lista ofrece al lector la posibilidad fÃ?Â¡cil y cÃ?Â³moda de profundizar en un tema concreto y de contextualizar a travÃ?Â©s de otros acontecimientos relacionados. TambiÃ?Â©n puedes utilizar este tipo de filtros para ver todos los artÃ?Â­culos de un tipo determinado, Ã?Â¡rea o tema de particular interÃ?Â©s para tÃ?Â­.<br /><br /></DIV>
<!-- </QuiÃ?Â©nes somos obj id 24> -->

<?php getPageLanguageOptions("ot", Array("0" => "en", "1" => "sp"), "24"); ?>
         <?php
          global $editor_session, $pseudo_directory_offset;
          if(($editor_session->canEditLayout()|| $editor_session->canEditContent()) && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobjdocument", 1)))
          {
          ?>
             <br>
            <a href="<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>?obj_id=1&amp;obj_type=document&amp;target_id=24&amp;action=edit&amp;site_id=1&amp;obj_language=ot&amp;edit_entry_url=[<?=$_REQUEST["obj_id"]?>][][][][][]"><img hspace=1 border=0  align="left" title="Edit Static Document [QuiÃ?Â©nes somos] Configuration" alt="Edit Static Document [QuiÃ?Â©nes somos] Configuration" src="<?php echo $pseudo_directory_offset?>graphics/mediaicons/32x32/editorial2.png" /></a>
             
          <?
          }
          ?>
          