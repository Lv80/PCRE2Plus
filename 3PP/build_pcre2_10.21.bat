REN pcre2-10.21
if not exist pcre.h copy pcre2.h.generic pcre2.h
if not exist config.h copy config.h.generic config.h
cl -MD -DHAVE_CONFIG_H dftables.c
dftables.exe pcre_chartables.c

set filelist=pcre2_find_bracket.c pcre2_auto_possess.c pcre_chartables.c pcre2_compile.c pcre2_config.c pcre2_context.c pcre2_dfa_match.c pcre2_error.c pcre2_jit_compile.c pcre2_maketables.c pcre2_match.c pcre2_match_data.c pcre2_newline.c pcre2_ord2utf.c pcre2_pattern_info.c pcre2_serialize.c pcre2_string_utils.c pcre2_study.c pcre2_substitute.c pcre2_substring.c pcre2_tables.c pcre2_ucd.c pcre2_valid_utf.c pcre2_xclass.c

set widths=8 16 32
for %%w in (%widths%) do (

    for %%a in (%filelist%) do (
       cl -DPCRE2_CODE_UNIT_WIDTH=%%w -DSUPPORT_UNICODE=1 -O1 -MD -DHAVE_CONFIG_H -c %%a
    )
    
    lib -out:libpcre2-%%w.lib pcre2_find_bracket.obj pcre2_auto_possess.obj pcre_chartables.obj pcre2_compile.obj pcre2_config.obj pcre2_context.obj pcre2_dfa_match.obj pcre2_error.obj pcre2_jit_compile.obj pcre2_maketables.obj pcre2_match.obj pcre2_match_data.obj pcre2_newline.obj pcre2_ord2utf.obj pcre2_pattern_info.obj pcre2_serialize.obj pcre2_string_utils.obj pcre2_study.obj pcre2_substitute.obj pcre2_substring.obj pcre2_tables.obj pcre2_ucd.obj pcre2_valid_utf.obj pcre2_xclass.obj

)