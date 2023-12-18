#include <glib.h>
#include <locale.h>
#include <string.h>
#include <stdlib.h>

static GKeyFile *
load_data (const gchar   *data, 
	   GKeyFileFlags  flags)
{
  GKeyFile *keyfile;
  GError *error = NULL;

  keyfile = g_key_file_new ();
  g_key_file_load_from_data (keyfile, data, -1, flags, &error);
  if (error)
    {
      g_print ("Could not load data: %s\n", error->message);
      exit (1);
    }
  
  return keyfile;
}

static void
check_error (GError **error,
	     GQuark   domain,
	     gint     code)
{
  if (*error == NULL)
    {
      g_print ("Missing an error\n");
      exit (1);
    }
  
  if ((*error)->domain != domain)
    {
      g_print ("Wrong error domain: got %s, expected %s\n",
	       g_quark_to_string ((*error)->domain),
	       g_quark_to_string (domain));
      exit (1);
    }
  
  if ((*error)->code != code)
    {
      g_print ("Wrong error code: got %d, expected %d\n",
	       (*error)->code, code);
      exit (1);
    }

  g_error_free (*error);
  *error = NULL;
}

static void 
check_no_error (GError **error)
{
  if (*error != NULL)
    {
      g_print ("Unexpected error: (%s, %d) %s\n",
	       g_quark_to_string ((*error)->domain),
	       (*error)->code, (*error)->message);
      exit (1);
    }
}

static void
check_string_value (GKeyFile    *keyfile,
		    const gchar *group,
		    const gchar *key,
		    const gchar *expected) 
{
  GError *error = NULL;
  gchar *value;

  value = g_key_file_get_string (keyfile, group, key, &error);
  check_no_error (&error);
  g_assert (value != NULL);

  if (strcmp (value, expected) != 0)
    {
      g_print ("Group %s key %s: "
	       "expected string value '%s', actual value '%s'\n",
	       group, key, expected, value);      
      exit (1);
    }

  g_free (value);
}

static void
check_locale_string_value (GKeyFile    *keyfile,
			   const gchar *group,
			   const gchar *key,
			   const gchar *locale,
			   const gchar *expected) 
{
  GError *error = NULL;
  gchar *value;

  value = g_key_file_get_locale_string (keyfile, group, key, locale, &error);
  check_no_error (&error);
  g_assert (value != NULL);

  if (strcmp (value, expected) != 0)
    {
      g_print ("Group %s key %s locale %s: "
	       "expected string value '%s', actual value '%s'\n",
	       group, key, locale, expected, value);      
      exit (1);
    }

  g_free (value);
}

static void
check_string_list_value (GKeyFile    *keyfile,
			 const gchar *group,
			 const gchar *key,
			 ...)
{
  gint i;
  gchar *v, **value;
  va_list args;
  gsize len;
  GError *error = NULL;

  value = g_key_file_get_string_list (keyfile, group, key, &len, &error);
  check_no_error (&error);
  g_assert (value != NULL);
  
  va_start (args, key);
  i = 0;
  v = va_arg (args, gchar*);
  while (v)
    {
      if (value[i] == NULL)
	{
	  g_print ("Group %s key %s: list too short (%d)\n", 
		   group, key, i);      
	  exit (1);
	}
      if (strcmp (v, value[i]) != 0)
	{
	  g_print ("Group %s key %s: mismatch at %d, expected %s, got %s\n", 
		   group, key, i, v, value[i]);      
	  exit (1);
	}

      i++;
      v = va_arg (args, gchar*);
    }

  va_end (args);
  
  g_strfreev (value);
}

static void
check_integer_list_value (GKeyFile    *keyfile,
			  const gchar *group,
			  const gchar *key,
			  ...)
{
  gint i;
  gint v, *value;
  va_list args;
  gsize len;
  GError *error = NULL;

  value = g_key_file_get_integer_list (keyfile, group, key, &len, &error);
  check_no_error (&error);
  g_assert (value != NULL);
  
  va_start (args, key);
  i = 0;
  v = va_arg (args, gint);
  while (v != -100)
    {
      if (i == len)
	{
	  g_print ("Group %s key %s: list too short (%d)\n", 
		   group, key, i);      
	  exit (1);
	}
      if (value[i] != v)
	{
	  g_print ("Group %s key %s: mismatch at %d, expected %d, got %d\n", 
		   group, key, i, v, value[i]);      
	  exit (1);
	}

      i++;
      v = va_arg (args, gint);
    }

  va_end (args);
  
  g_free (value);
}

static void
check_double_list_value (GKeyFile    *keyfile,
			  const gchar *group,
			  const gchar *key,
			  ...)
{
  gint i;
  gdouble v, *value;
  va_list args;
  gsize len;
  GError *error = NULL;

  value = g_key_file_get_double_list (keyfile, group, key, &len, &error);
  check_no_error (&error);
  g_assert (value != NULL);
  
  va_start (args, key);
  i = 0;
  v = va_arg (args, gdouble);
  while (v != -100)
    {
      if (i == len)
	{
	  g_print ("Group %s key %s: list too short (%d)\n", 
		   group, key, i);      
	  exit (1);
	}
      if (value[i] != v)
	{
	  g_print ("Group %s key %s: mismatch at %d, expected %e, got %e\n", 
		   group, key, i, v, value[i]);      
	  exit (1);
	}

      i++;
      v = va_arg (args, gdouble);
    }

  va_end (args);
  
  g_free (value);
}

static void
check_boolean_list_value (GKeyFile    *keyfile,
			  const gchar *group,
			  const gchar *key,
			  ...)
{
  gint i;
  gboolean v, *value;
  va_list args;
  gsize len;
  GError *error = NULL;

  value = g_key_file_get_boolean_list (keyfile, group, key, &len, &error);
  check_no_error (&error);
  g_assert (value != NULL);
  
  va_start (args, key);
  i = 0;
  v = va_arg (args, gboolean);
  while (v != -100)
    {
      if (i == len)
	{
	  g_print ("Group %s key %s: list too short (%d)\n", 
		   group, key, i);      
	  exit (1);
	}
      if (value[i] != v)
	{
	  g_print ("Group %s key %s: mismatch at %d, expected %d, got %d\n", 
		   group, key, i, v, value[i]);      
	  exit (1);
	}

      i++;
      v = va_arg (args, gboolean);
    }

  va_end (args);
  
  g_free (value);
}

static void
check_boolean_value (GKeyFile    *keyfile,
		     const gchar *group,
		     const gchar *key,
		     gboolean     expected) 
{
  GError *error = NULL;
  gboolean value;

  value = g_key_file_get_boolean (keyfile, group, key, &error);
  check_no_error (&error);

  if (value != expected)
    {
      g_print ("Group %s key %s: "
	       "expected boolean value '%s', actual value '%s'\n",
	       group, key, 
	       expected ? "true" : "false", 
	       value ? "true" : "false");      
      exit (1);
    }
}

static void
check_integer_value (GKeyFile    *keyfile,
		     const gchar *group,
		     const gchar *key,
		     gint         expected) 
{
  GError *error = NULL;
  gint value;

  value = g_key_file_get_integer (keyfile, group, key, &error);
  check_no_error (&error);

  if (value != expected)
    {
      g_print ("Group %s key %s: "
	       "expected integer value %d, actual value %d\n",
	       group, key, expected, value);      
      exit (1);
    }
}

static void
check_double_value (GKeyFile    *keyfile,
		     const gchar *group,
		     const gchar *key,
		     gdouble      expected) 
{
  GError *error = NULL;
  gdouble value;

  value = g_key_file_get_double (keyfile, group, key, &error);
  check_no_error (&error);

  if (value != expected)
    {
      g_print ("Group %s key %s: "
	       "expected integer value %e, actual value %e\n",
	       group, key, expected, value);      
      exit (1);
    }
}

static void
check_name (const gchar *what,
	    const gchar *value,
	    const gchar *expected,
	    gint         position)
{
  if (!value || strcmp (expected, value) != 0)
    {
      g_print ("Wrong %s returned: got '%s' at %d, expected '%s'\n",
	       what, value, position, expected);
      exit (1);
    }
}

static void
check_length (const gchar *what,
	      gint         n_items,
	      gint         length,
	      gint         expected)
{
  if (n_items != length || length != expected)
    {
      g_print ("Wrong number of %s returned: got %d items, length %d, expected %d\n",
	       what, n_items, length, expected);
      exit (1);
    }
}


/* check that both \n and \r\n are accepted as line ends,
 * and that stray \r are passed through
 */
static void
test_line_ends (void)
{
  GKeyFile *keyfile;

  const gchar *data = 
    "[group1]\n"
    "key1=value1\n"
    "key2=value2\r\n"
    "[group2]\r\n"
    "key3=value3\r\r\n"
    "key4=value4\n";

  keyfile = load_data (data, 0);

  check_string_value (keyfile, "group1", "key1", "value1");
  check_string_value (keyfile, "group1", "key2", "value2");
  check_string_value (keyfile, "group2", "key3", "value3\r");
  check_string_value (keyfile, "group2", "key4", "value4");

  g_key_file_free (keyfile);
}

/* check handling of whitespace 
 */
static void
test_whitespace (void)
{
  GKeyFile *keyfile;

  const gchar *data = 
    "[group1]\n"
    "key1 = value1\n"
    "key2\t=\tvalue2\n"
    " [ group2 ] \n"
    "key3  =  value3  \n"
    "key4  =  value \t4\n"
    "  key5  =  value5\n";
  
  keyfile = load_data (data, 0);

  check_string_value (keyfile, "group1", "key1", "value1");
  check_string_value (keyfile, "group1", "key2", "value2");
  check_string_value (keyfile, " group2 ", "key3", "value3  ");
  check_string_value (keyfile, " group2 ", "key4", "value \t4");
  check_string_value (keyfile, " group2 ", "key5", "value5");

  g_key_file_free (keyfile);
}

/* check handling of comments
 */
static void
test_comments (void)
{
  GKeyFile *keyfile;
  gchar **names;
  gsize len;
  GError *error = NULL;
  gchar *comment;

  const gchar *data = 
    "# top comment\n"
    "# top comment, continued\n"
    "[group1]\n"
    "key1 = value1\n"
    "# key comment\n"
    "# key comment, continued\n"
    "key2 = value2\n"
    "# line end check\r\n"
    "key3 = value3\n"
    "key4 = value4\n"
    "# group comment\n"
    "# group comment, continued\n"
    "[group2]\n";

  const gchar *top_comment= " top comment\n top comment, continued\n";
  const gchar *group_comment= " group comment\n group comment, continued\n";
  const gchar *key_comment= " key comment\n key comment, continued\n";
  
  keyfile = load_data (data, 0);

  check_string_value (keyfile, "group1", "key1", "value1");
  check_string_value (keyfile, "group1", "key2", "value2");
  check_string_value (keyfile, "group1", "key3", "value3");
  check_string_value (keyfile, "group1", "key4", "value4");

  names = g_key_file_get_keys (keyfile, "group1", &len, &error);
  check_no_error (&error);

  check_length ("keys", g_strv_length (names), len, 4);
  check_name ("key", names[0], "key1", 0);
  check_name ("key", names[1], "key2", 1);
  check_name ("key", names[2], "key3", 2);
  check_name ("key", names[3], "key4", 3);

  g_strfreev (names);

  g_key_file_free (keyfile);

  keyfile = load_data (data, G_KEY_FILE_KEEP_COMMENTS);

  names = g_key_file_get_keys (keyfile, "group1", &len, &error);
  check_no_error (&error);

  check_length ("keys", g_strv_length (names), len, 4);
  check_name ("key", names[0], "key1", 0);
  check_name ("key", names[1], "key2", 1);
  check_name ("key", names[2], "key3", 2);
  check_name ("key", names[3], "key4", 3);

  g_strfreev (names);

  comment = g_key_file_get_comment (keyfile, NULL, NULL, &error);
  check_no_error (&error);
  check_name ("top comment", comment, top_comment, 0);
  g_free (comment);

  comment = g_key_file_get_comment (keyfile, "group1", "key2", &error);
  check_no_error (&error);
  check_name ("key comment", comment, key_comment, 0);
  g_free (comment);

  comment = g_key_file_get_comment (keyfile, "group2", NULL, &error);
  check_no_error (&error);
  check_name ("group comment", comment, group_comment, 0);
  g_free (comment);

  comment = g_key_file_get_comment (keyfile, "group3", NULL, &error);
  check_error (&error, 
	       G_KEY_FILE_ERROR,
	       G_KEY_FILE_ERROR_GROUP_NOT_FOUND);
  g_assert (comment == NULL);

  g_key_file_free (keyfile);
}


/* check key and group listing */
static void
test_listing (void)
{
  GKeyFile *keyfile;
  gchar **names;
  gsize len;
  gchar *start;
  GError *error = NULL;

  const gchar *data = 
    "[group1]\n"
    "key1=value1\n"
    "key2=value2\n"
    "[group2]\n"
    "key3=value3\n"
    "key4=value4\n";
  
  keyfile = load_data (data, 0);

  names = g_key_file_get_groups (keyfile, &len);
  if (names == NULL)
    {
      g_print ("Error listing groups\n");
      exit (1);
    }

  check_length ("groups", g_strv_length (names), len, 2);
  check_name ("group name", names[0], "group1", 0);
  check_name ("group name", names[1], "group2", 1);
  
  g_strfreev (names);
  
  names = g_key_file_get_keys (keyfile, "group1", &len, &error);
  check_no_error (&error);

  check_length ("keys", g_strv_length (names), len, 2);
  check_name ("key", names[0], "key1", 0);
  check_name ("key", names[1], "key2", 1);

  g_strfreev (names);

  names = g_key_file_get_keys (keyfile, "no-such-group", &len, &error);
  check_error (&error, G_KEY_FILE_ERROR, G_KEY_FILE_ERROR_GROUP_NOT_FOUND);

  g_strfreev (names);

  if (!g_key_file_has_group (keyfile, "group1") ||
      !g_key_file_has_group (keyfile, "group2") ||
      g_key_file_has_group (keyfile, "group10") ||
      g_key_file_has_group (keyfile, "group2 "))      
    {
      g_print ("Group finding trouble\n");
      exit (1);      
    }

  start = g_key_file_get_start_group (keyfile);
  if (!start || strcmp (start, "group1") != 0)
    {
      g_print ("Start group finding trouble\n");
      exit (1);
    }
  g_free (start);

  if (!g_key_file_has_key (keyfile, "group1", "key1", &error) ||
      !g_key_file_has_key (keyfile, "group2", "key3", &error) ||
      g_key_file_has_key (keyfile, "group2", "no-such-key", &error))
    {
      g_print ("Key finding trouble\n");
      exit (1);      
    }
  check_no_error (&error);
  
  g_key_file_has_key (keyfile, "no-such-group", "key", &error);
  check_error (&error, G_KEY_FILE_ERROR, G_KEY_FILE_ERROR_GROUP_NOT_FOUND);

  g_key_file_free (keyfile);
}

/* check parsing of string values */
static void
test_string (void)
{
  GKeyFile *keyfile;
  GError *error = NULL;
  gchar *value;

  const gchar *data = 
    "[valid]\n"
    "key1=\\s\\n\\t\\r\\\\\n"
    "key2=\"quoted\"\n"
    "key3='quoted'\n"
    "key4=\xe2\x89\xa0\xe2\x89\xa0\n"
    "[invalid]\n"
    "key1=\\a\\b\\0800xff\n"
    "key2=blabla\\\n";
  
  keyfile = load_data (data, 0);

  check_string_value (keyfile, "valid", "key1", " \n\t\r\\");
  check_string_value (keyfile, "valid", "key2", "\"quoted\"");
  check_string_value (keyfile, "valid", "key3", "'quoted'");  
  check_string_value (keyfile, "valid", "key4", "\xe2\x89\xa0\xe2\x89\xa0");  
  
  value = g_key_file_get_string (keyfile, "invalid", "key1", &error);
  check_error (&error, G_KEY_FILE_ERROR, G_KEY_FILE_ERROR_INVALID_VALUE);
  g_free (value);

  value = g_key_file_get_string (keyfile, "invalid", "key2", &error);
  check_error (&error, G_KEY_FILE_ERROR, G_KEY_FILE_ERROR_INVALID_VALUE);
  g_free (value);
  
  g_key_file_free (keyfile);
}

/* check parsing of boolean values */
static void
test_boolean (void)
{
  GKeyFile *keyfile;
  GError *error = NULL;

  const gchar *data = 
    "[valid]\n"
    "key1=true\n"
    "key2=false\n"
    "key3=1\n"
    "key4=0\n"
    "[invalid]\n"
    "key1=t\n"
    "key2=f\n"
    "key3=yes\n"
    "key4=no\n";
  
  keyfile = load_data (data, 0);

  check_boolean_value (keyfile, "valid", "key1", TRUE);
  check_boolean_value (keyfile, "valid", "key2", FALSE);
  check_boolean_value (keyfile, "valid", "key3", TRUE);
  check_boolean_value (keyfile, "valid", "key4", FALSE);

  g_key_file_get_boolean (keyfile, "invalid", "key1", &error);
  check_error (&error, G_KEY_FILE_ERROR, G_KEY_FILE_ERROR_INVALID_VALUE);

  g_key_file_get_boolean (keyfile, "invalid", "key2", &error);
  check_error (&error, G_KEY_FILE_ERROR, G_KEY_FILE_ERROR_INVALID_VALUE);

  g_key_file_get_boolean (keyfile, "invalid", "key3", &error);
  check_error (&error, G_KEY_FILE_ERROR, G_KEY_FILE_ERROR_INVALID_VALUE);

  g_key_file_get_boolean (keyfile, "invalid", "key4", &error);
  check_error (&error, G_KEY_FILE_ERROR, G_KEY_FILE_ERROR_INVALID_VALUE);

  g_key_file_free (keyfile);
}

/* check parsing of integer and double values */
static void
test_number (void)
{
  GKeyFile *keyfile;
  GError *error = NULL;

  const gchar *data = 
    "[valid]\n"
    "key1=0\n"
    "key2=1\n"
    "key3=-1\n"
    "key4=2324431\n"
    "key5=-2324431\n"
    "key6=000111\n"
    "dkey1=000111\n"
    "dkey2=145.45\n"
    "dkey3=-3453.7\n"
    "[invalid]\n"
    "key1=0xffff\n"
    "key2=0.5\n"
    "key3=1e37\n"
    "key4=ten\n"
    "key5=\n"
    "key6=1.0.0\n"
    "key7=2x2\n"
    "key8=abc\n";
  
  keyfile = load_data (data, 0);

  check_integer_value (keyfile, "valid", "key1", 0);
  check_integer_value (keyfile, "valid", "key2", 1);
  check_integer_value (keyfile, "valid", "key3", -1);
  check_integer_value (keyfile, "valid", "key4", 2324431);
  check_integer_value (keyfile, "valid", "key5", -2324431);
  check_integer_value (keyfile, "valid", "key6", 111);
  check_double_value (keyfile, "valid", "dkey1", 111.0);
  check_double_value (keyfile, "valid", "dkey2", 145.45);
  check_double_value (keyfile, "valid", "dkey3", -3453.7);

  g_key_file_get_integer (keyfile, "invalid", "key1", &error);
  check_error (&error, G_KEY_FILE_ERROR, G_KEY_FILE_ERROR_INVALID_VALUE);

  g_key_file_get_integer (keyfile, "invalid", "key2", &error);
  check_error (&error, G_KEY_FILE_ERROR, G_KEY_FILE_ERROR_INVALID_VALUE);

  g_key_file_get_integer (keyfile, "invalid", "key3", &error);
  check_error (&error, G_KEY_FILE_ERROR, G_KEY_FILE_ERROR_INVALID_VALUE);

  g_key_file_get_integer (keyfile, "invalid", "key4", &error);
  check_error (&error, G_KEY_FILE_ERROR, G_KEY_FILE_ERROR_INVALID_VALUE);

  g_key_file_get_double (keyfile, "invalid", "key5", &error);
  check_error (&error, G_KEY_FILE_ERROR, G_KEY_FILE_ERROR_INVALID_VALUE);

  g_key_file_get_double (keyfile, "invalid", "key6", &error);
  check_error (&error, G_KEY_FILE_ERROR, G_KEY_FILE_ERROR_INVALID_VALUE);

  g_key_file_get_double (keyfile, "invalid", "key7", &error);
  check_error (&error, G_KEY_FILE_ERROR, G_KEY_FILE_ERROR_INVALID_VALUE);

  g_key_file_get_double (keyfile, "invalid", "key8", &error);
  check_error (&error, G_KEY_FILE_ERROR, G_KEY_FILE_ERROR_INVALID_VALUE);

  g_key_file_free (keyfile);
}

/* check handling of translated strings */
static void
test_locale_string (void)
{
  GKeyFile *keyfile;

  const gchar *data = 
    "[valid]\n"
    "key1=v1\n"
    "key1[de]=v1-de\n"
    "key1[de_DE]=v1-de_DE\n"
    "key1[de_DE.UTF8]=v1-de_DE.UTF8\n"
    "key1[fr]=v1-fr\n"
    "key1[en] =v1-en\n"
    "key1[sr@Latn]=v1-sr\n";
  
  keyfile = load_data (data, G_KEY_FILE_KEEP_TRANSLATIONS);

  check_locale_string_value (keyfile, "valid", "key1", "it", "v1");
  check_locale_string_value (keyfile, "valid", "key1", "de", "v1-de");
  check_locale_string_value (keyfile, "valid", "key1", "de_DE", "v1-de_DE");
  check_locale_string_value (keyfile, "valid", "key1", "de_DE.UTF8", "v1-de_DE.UTF8");
  check_locale_string_value (keyfile, "valid", "key1", "fr", "v1-fr");
  check_locale_string_value (keyfile, "valid", "key1", "fr_FR", "v1-fr");
  check_locale_string_value (keyfile, "valid", "key1", "en", "v1-en");
  check_locale_string_value (keyfile, "valid", "key1", "sr@Latn", "v1-sr");
  
  g_key_file_free (keyfile);

  /* now test that translations are thrown away */

  g_setenv ("LANGUAGE", "de", TRUE);
  setlocale (LC_ALL, "");

  keyfile = load_data (data, 0);

  check_locale_string_value (keyfile, "valid", "key1", "it", "v1");
  check_locale_string_value (keyfile, "valid", "key1", "de", "v1-de");
  check_locale_string_value (keyfile, "valid", "key1", "de_DE", "v1-de");
  check_locale_string_value (keyfile, "valid", "key1", "de_DE.UTF8", "v1-de");
  check_locale_string_value (keyfile, "valid", "key1", "fr", "v1");
  check_locale_string_value (keyfile, "valid", "key1", "fr_FR", "v1");
  check_locale_string_value (keyfile, "valid", "key1", "en", "v1");

  g_key_file_free (keyfile);  
}

static void
test_lists (void)
{
  GKeyFile *keyfile;

  const gchar *data = 
    "[valid]\n"
    "key1=v1;v2\n"
    "key2=v1;v2;\n"
    "key3=v1,v2\n"
    "key4=v1\\;v2\n"
    "key5=true;false\n"
    "key6=1;0;-1\n"
    "key7= 1 ; 0 ; -1 \n"
    "key8=v1\\,v2\n"
    "key9=0;1.3456;-76532.456\n";
  
  keyfile = load_data (data, 0);

  check_string_list_value (keyfile, "valid", "key1", "v1", "v2", NULL);
  check_string_list_value (keyfile, "valid", "key2", "v1", "v2", NULL);
  check_string_list_value (keyfile, "valid", "key3", "v1,v2", NULL);
  check_string_list_value (keyfile, "valid", "key4", "v1;v2", NULL);
  check_boolean_list_value (keyfile, "valid", "key5", TRUE, FALSE, -100);
  check_integer_list_value (keyfile, "valid", "key6", 1, 0, -1, -100);
  check_double_list_value (keyfile, "valid", "key9", 0.0, 1.3456, -76532.456, -100.0);
  /* maybe these should be valid */
  /* check_integer_list_value (keyfile, "valid", "key7", 1, 0, -1, -100);*/
  /* check_string_list_value (keyfile, "valid", "key8", "v1\\,v2", NULL);*/

  g_key_file_free (keyfile);  

  /* Now check an alternate separator */

  keyfile = load_data (data, 0);
  g_key_file_set_list_separator (keyfile, ',');

  check_string_list_value (keyfile, "valid", "key1", "v1;v2", NULL);
  check_string_list_value (keyfile, "valid", "key2", "v1;v2;", NULL);
  check_string_list_value (keyfile, "valid", "key3", "v1", "v2", NULL);

  g_key_file_free (keyfile);  
}

/* http://bugzilla.gnome.org/show_bug.cgi?id=165887 */
static void 
test_group_remove (void)
{
  GKeyFile *keyfile;
  gchar **names;
  gsize len;
  GError *error = NULL;

  const gchar *data = 
    "[group1]\n"
    "[group2]\n"
    "key1=bla\n"
    "key2=bla\n"
    "[group3]\n"
    "key1=bla\n"
    "key2=bla\n";
  
  keyfile = load_data (data, 0);
  
  names = g_key_file_get_groups (keyfile, &len);
  if (names == NULL)
    {
      g_print ("Error listing groups\n");
      exit (1);
    }

  check_length ("groups", g_strv_length (names), len, 3);
  check_name ("group name", names[0], "group1", 0);
  check_name ("group name", names[1], "group2", 1);
  check_name ("group name", names[2], "group3", 2);

  g_key_file_remove_group (keyfile, "group1", &error);
  check_no_error (&error);
  
  g_strfreev (names);

  names = g_key_file_get_groups (keyfile, &len);
  if (names == NULL)
    {
      g_print ("Error listing groups\n");
      exit (1);
    }

  check_length ("groups", g_strv_length (names), len, 2);
  check_name ("group name", names[0], "group2", 0);
  check_name ("group name", names[1], "group3", 1);

  g_key_file_remove_group (keyfile, "group2", &error);
  check_no_error (&error);
  
  g_strfreev (names);

  names = g_key_file_get_groups (keyfile, &len);
  if (names == NULL)
    {
      g_print ("Error listing groups\n");
      exit (1);
    }

  check_length ("groups", g_strv_length (names), len, 1);
  check_name ("group name", names[0], "group3", 0);

  g_key_file_remove_group (keyfile, "no such group", &error);
  check_error (&error, G_KEY_FILE_ERROR, G_KEY_FILE_ERROR_GROUP_NOT_FOUND);

  g_strfreev (names);

  g_key_file_free (keyfile);
}

/* http://bugzilla.gnome.org/show_bug.cgi?id=165980 */
static void 
test_key_remove (void)
{
  GKeyFile *keyfile;
  gchar *value;
  GError *error = NULL;

  const gchar *data = 
    "[group1]\n"
    "key1=bla\n"
    "key2=bla\n";
  
  keyfile = load_data (data, 0);
  
  check_string_value (keyfile, "group1", "key1", "bla");

  g_key_file_remove_key (keyfile, "group1", "key1", &error);
  check_no_error (&error);

  value = g_key_file_get_string (keyfile, "group1", "key1", &error);
  check_error (&error, G_KEY_FILE_ERROR, G_KEY_FILE_ERROR_KEY_NOT_FOUND);
  g_free (value);
  
  g_key_file_remove_key (keyfile, "group1", "key1", &error);
  check_error (&error, G_KEY_FILE_ERROR, G_KEY_FILE_ERROR_KEY_NOT_FOUND);

  g_key_file_remove_key (keyfile, "no such group", "key1", &error);
  check_error (&error, G_KEY_FILE_ERROR, G_KEY_FILE_ERROR_GROUP_NOT_FOUND);

  g_key_file_free (keyfile);
}


/* http://bugzilla.gnome.org/show_bug.cgi?id=316309 */
static void
test_groups (void)
{
  GKeyFile *keyfile;

  const gchar *data = 
    "[1]\n"
    "key1=123\n"
    "[2]\n"
    "key2=123\n";
  
  keyfile = load_data (data, 0);

  check_string_value (keyfile, "1", "key1", "123");
  check_string_value (keyfile, "2", "key2", "123");

  g_key_file_free (keyfile);  
}

static void
test_group_names (void)
{
  GKeyFile *keyfile;
  GError *error = NULL;
  const gchar *data;
  gchar *value;

  /* [ in group name */
  data = "[a[b]\n"
         "key1=123\n";
  keyfile = g_key_file_new ();
  g_key_file_load_from_data (keyfile, data, -1, 0, &error);
  g_key_file_free (keyfile);  
  check_error (&error, 
	       G_KEY_FILE_ERROR,
	       G_KEY_FILE_ERROR_PARSE);

  /* ] in group name */
  data = "[a]b]\n"
         "key1=123\n";
  keyfile = g_key_file_new ();
  g_key_file_load_from_data (keyfile, data, -1, 0, &error);
  g_key_file_free (keyfile);  
  check_error (&error, 
	       G_KEY_FILE_ERROR,
	       G_KEY_FILE_ERROR_PARSE);

  /* control char in group name */
  data = "[a\tb]\n"
         "key1=123\n";
  keyfile = g_key_file_new ();
  g_key_file_load_from_data (keyfile, data, -1, 0, &error);
  g_key_file_free (keyfile);  
  check_error (&error, 
	       G_KEY_FILE_ERROR,
	       G_KEY_FILE_ERROR_PARSE);

  /* empty group name */
  data = "[]\n"
         "key1=123\n";
  keyfile = g_key_file_new ();
  g_key_file_load_from_data (keyfile, data, -1, 0, &error);
  g_key_file_free (keyfile);  
  check_error (&error, 
	       G_KEY_FILE_ERROR,
	       G_KEY_FILE_ERROR_PARSE);

  /* Unicode in group name */
  data = "[\xc2\xbd]\n"
         "key1=123\n";
  keyfile = g_key_file_new ();
  g_key_file_load_from_data (keyfile, data, -1, 0, &error);
  g_key_file_free (keyfile);  
  check_no_error (&error);

  keyfile = g_key_file_new ();
  g_key_file_set_string (keyfile, "a[b", "key1", "123");
  value = g_key_file_get_string (keyfile, "a[b", "key1", &error);
  check_error (&error, 
               G_KEY_FILE_ERROR,
               G_KEY_FILE_ERROR_GROUP_NOT_FOUND);  
  g_key_file_free (keyfile);  

  keyfile = g_key_file_new ();
  g_key_file_set_string (keyfile, "a]b", "key1", "123");
  value = g_key_file_get_string (keyfile, "a]b", "key1", &error);
  check_error (&error, 
               G_KEY_FILE_ERROR,
               G_KEY_FILE_ERROR_GROUP_NOT_FOUND);  
  g_key_file_free (keyfile);  

  keyfile = g_key_file_new ();
  g_key_file_set_string (keyfile, "a\tb", "key1", "123");
  value = g_key_file_get_string (keyfile, "a\tb", "key1", &error);
  check_error (&error, 
               G_KEY_FILE_ERROR,
               G_KEY_FILE_ERROR_GROUP_NOT_FOUND);  
  g_key_file_free (keyfile);  

  keyfile = g_key_file_new ();
  g_key_file_set_string (keyfile, "\xc2\xbd", "key1", "123");
  check_string_value (keyfile, "\xc2\xbd", "key1", "123");
  g_key_file_free (keyfile);  
}

static void
test_key_names (void)
{
  GKeyFile *keyfile;
  GError *error = NULL;
  const gchar *data;
  gchar *value;

  /* [ in key name */
  data = "[a]\n"
         "key[=123\n";
  keyfile = g_key_file_new ();
  g_key_file_load_from_data (keyfile, data, -1, 0, &error);
  g_key_file_free (keyfile);  
  check_error (&error, 
	       G_KEY_FILE_ERROR,
	       G_KEY_FILE_ERROR_PARSE);

  /* empty key name */
  data = "[a]\n"
         " =123\n";
  keyfile = g_key_file_new ();
  g_key_file_load_from_data (keyfile, data, -1, 0, &error);
  g_key_file_free (keyfile);  
  check_error (&error, 
	       G_KEY_FILE_ERROR,
	       G_KEY_FILE_ERROR_PARSE);

  /* empty key name */
  data = "[a]\n"
         " [de] =123\n";
  keyfile = g_key_file_new ();
  g_key_file_load_from_data (keyfile, data, -1, 0, &error);
  g_key_file_free (keyfile);  
  check_error (&error, 
	       G_KEY_FILE_ERROR,
	       G_KEY_FILE_ERROR_PARSE);

  /* bad locale suffix */
  data = "[a]\n"
         "foo[@#!&%]=123\n";
  keyfile = g_key_file_new ();
  g_key_file_load_from_data (keyfile, data, -1, 0, &error);
  g_key_file_free (keyfile);  
  check_error (&error, 
	       G_KEY_FILE_ERROR,
	       G_KEY_FILE_ERROR_PARSE);

  /* initial space */
  data = "[a]\n"
         " foo=123\n";
  keyfile = g_key_file_new ();
  g_key_file_load_from_data (keyfile, data, -1, 0, &error);
  check_no_error (&error);
  check_string_value (keyfile, "a", "foo", "123");
  g_key_file_free (keyfile);  

  /* final space */
  data = "[a]\n"
         "foo =123\n";
  keyfile = g_key_file_new ();
  g_key_file_load_from_data (keyfile, data, -1, 0, &error);
  check_no_error (&error);
  check_string_value (keyfile, "a", "foo", "123");
  g_key_file_free (keyfile);  

  /* inner space */
  data = "[a]\n"
         "foo bar=123\n";
  keyfile = g_key_file_new ();
  g_key_file_load_from_data (keyfile, data, -1, 0, &error);
  check_no_error (&error);
  check_string_value (keyfile, "a", "foo bar", "123");
  g_key_file_free (keyfile);  

  /* inner space */
  data = "[a]\n"
         "foo [de] =123\n";
  keyfile = g_key_file_new ();
  g_key_file_load_from_data (keyfile, data, -1, 0, &error);
  check_error (&error, 
	       G_KEY_FILE_ERROR,
	       G_KEY_FILE_ERROR_PARSE);
  g_key_file_free (keyfile);  

  /* control char in key name */
  data = "[a]\n"
         "key\tfoo=123\n";
  keyfile = g_key_file_new ();
  g_key_file_load_from_data (keyfile, data, -1, 0, &error);
  g_key_file_free (keyfile);  
  check_no_error (&error);

  /* Unicode in key name */
  data = "[a]\n"
         "\xc2\xbd=123\n";
  keyfile = g_key_file_new ();
  g_key_file_load_from_data (keyfile, data, -1, 0, &error);
  g_key_file_free (keyfile);  
  check_no_error (&error); 

  keyfile = g_key_file_new ();
  g_key_file_set_string (keyfile, "a", "x", "123");
  g_key_file_set_string (keyfile, "a", "key=", "123");
  value = g_key_file_get_string (keyfile, "a", "key=", &error);
  check_error (&error, 
               G_KEY_FILE_ERROR,
               G_KEY_FILE_ERROR_KEY_NOT_FOUND);  
  g_key_file_free (keyfile);  

  keyfile = g_key_file_new ();
  g_key_file_set_string (keyfile, "a", "x", "123");
  g_key_file_set_string (keyfile, "a", "key[", "123");
  value = g_key_file_get_string (keyfile, "a", "key[", &error);
  check_error (&error, 
               G_KEY_FILE_ERROR,
               G_KEY_FILE_ERROR_KEY_NOT_FOUND);  
  g_key_file_free (keyfile);  

  keyfile = g_key_file_new ();
  g_key_file_set_string (keyfile, "a", "x", "123");
  g_key_file_set_string (keyfile, "a", "key\tfoo", "123");
  value = g_key_file_get_string (keyfile, "a", "key\tfoo", &error);
  check_no_error (&error);
  g_key_file_free (keyfile);  

  keyfile = g_key_file_new ();
  g_key_file_set_string (keyfile, "a", "x", "123");
  g_key_file_set_string (keyfile, "a", " key", "123");
  value = g_key_file_get_string (keyfile, "a", " key", &error);
  check_error (&error, 
               G_KEY_FILE_ERROR,
               G_KEY_FILE_ERROR_KEY_NOT_FOUND);  
  g_key_file_free (keyfile);  

  keyfile = g_key_file_new ();
  g_key_file_set_string (keyfile, "a", "x", "123");

  /* Unicode key */
  g_key_file_set_string (keyfile, "a", "\xc2\xbd", "123");
  check_string_value (keyfile, "a", "\xc2\xbd", "123");

  /* Keys with / + . (as used by the gnome-vfs mime cache) */
  g_key_file_set_string (keyfile, "a", "foo/bar", "/");
  check_string_value (keyfile, "a", "foo/bar", "/");
  g_key_file_set_string (keyfile, "a", "foo+bar", "+");
  check_string_value (keyfile, "a", "foo+bar", "+");
  g_key_file_set_string (keyfile, "a", "foo.bar", ".");
  check_string_value (keyfile, "a", "foo.bar", ".");

  g_key_file_free (keyfile);  
}

static void
test_duplicate_keys (void)
{
  GKeyFile *keyfile;
  const gchar *data = 
    "[1]\n"
    "key1=123\n"
    "key1=345\n";

  keyfile = load_data (data, 0);
  check_string_value (keyfile, "1", "key1", "345");

  g_key_file_free (keyfile);  
}

/* http://bugzilla.gnome.org/show_bug.cgi?id=157877 */
static void
test_duplicate_groups (void)
{
  GKeyFile *keyfile;
  const gchar *data = 
    "[Desktop Entry]\n"
    "key1=123\n"
    "[Desktop Entry]\n"
    "key2=123\n";
  
  keyfile = load_data (data, 0);
  check_string_value (keyfile, "Desktop Entry", "key1", "123");
  check_string_value (keyfile, "Desktop Entry", "key2", "123");

  g_key_file_free (keyfile);  
}

/* http://bugzilla.gnome.org/show_bug.cgi?id=385910 */
static void
test_duplicate_groups2 (void)
{
  GKeyFile *keyfile;
  const gchar *data = 
    "[A]\n"
    "foo=bar\n"
    "[B]\n"
    "foo=baz\n"
    "[A]\n"
    "foo=bang\n";
  
  keyfile = load_data (data, 0);
  check_string_value (keyfile, "A", "foo", "bang");
  check_string_value (keyfile, "B", "foo", "baz");

  g_key_file_free (keyfile);  
}


/* http://bugzilla.gnome.org/show_bug.cgi?id=420686 */
static void
test_reload_idempotency (void)
{
  static const gchar *original_data=""
    "# Top comment\n"
    "\n"
    "# First comment\n"
    "[first]\n"
    "key=value\n"
    "# A random comment in the first group\n"
    "anotherkey=anothervalue\n"
    "# Second comment - one line\n"
    "[second]\n"
    "# Third comment - two lines\n"
    "# Third comment - two lines\n"
    "[third]\n"
    "blank_line=1\n"
    "\n"
    "blank_lines=2\n"
    "\n\n"
    "[fourth]\n"
    "[fifth]\n";
  GKeyFile *keyfile;
  GError *error = NULL;
  gchar *data1, *data2;
  gsize len1, len2;

  /* check that we only insert a single new line between groups */
  keyfile = g_key_file_new ();
  if (!g_key_file_load_from_data (keyfile,
	                          original_data, strlen(original_data),
	                          G_KEY_FILE_KEEP_COMMENTS,
	                          &error)) {
    g_print ("Failed to parse keyfile[1]: %s", error->message);
    g_error_free (error);
    exit (1);
  }

  data1 = g_key_file_to_data (keyfile, &len1, &error);
  if (data1 == NULL) {
    g_print ("Failed to extract keyfile[1]: %s", error->message);
    g_error_free (error);
    exit (1);
  }
  g_key_file_free (keyfile);

  keyfile = g_key_file_new ();
  if (!g_key_file_load_from_data (keyfile,
	                          data1, len1,
				  G_KEY_FILE_KEEP_COMMENTS,
				  &error)) {
    g_print ("Failed to parse keyfile[2]: %s", error->message);
    g_error_free (error);
    exit (1);
  }

  data2 = g_key_file_to_data (keyfile, &len2, &error);
  if (data2 == NULL) {
    g_print ("Failed to extract keyfile[2]: %s", error->message);
    g_error_free (error);
    exit (1);
  }
  g_key_file_free (keyfile);


  if (strcmp(data1, data2) != 0) {
    g_print ("Reloading GKeyFile is not idempotent.");
    g_print ("original:\n%s\n---\n", original_data);
    g_print ("pass1:\n%s\n---\n", data1);
    g_print ("pass2:\n%s\n---\n", data2);
    exit (1);
  }

  g_free (data2);
  g_free (data1);
}


static void 
log_func (const gchar   *log_domain,
          GLogLevelFlags log_level,
          const gchar   *message,
          gpointer       user_data)
{
}

int
main (int argc, char *argv[])
{
  g_log_set_default_handler (log_func, NULL);

  test_line_ends ();
  test_whitespace ();
  test_comments ();
  test_listing ();
  test_string ();
  test_boolean ();
  test_number ();
  test_locale_string ();
  test_lists ();
  test_group_remove ();
  test_key_remove ();
  test_groups ();
  test_duplicate_keys ();
  test_duplicate_groups ();
  test_duplicate_groups2 ();
  test_group_names ();
  test_key_names ();
  test_reload_idempotency ();
  
  return 0;
}
