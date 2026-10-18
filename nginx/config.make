cat << END                                            >> $NGX_MAKEFILE

$ngx_addon_dir/../build/libnjs.a: $NGX_MAKEFILE
	cd $ngx_addon_dir/.. \\
	&& if [ -f build/Makefile ]; then \$(MAKE) clean; fi \\
	&& CFLAGS="\$(CFLAGS)" CC="\$(CC)" ./configure --no-openssl \\
		--no-libxml2 --no-zlib --no-pcre --no-quickjs --no-fs \\
		--ld-opt="$NGX_LD_OPT" \\
	&& \$(MAKE) libnjs

$ngx_addon_dir/../build/libqjs.a: $NGX_MAKEFILE
	cd $ngx_addon_dir/.. \\
	&& if [ -f build/Makefile ]; then \$(MAKE) clean; fi \\
	&& CFLAGS="\$(CFLAGS)" CC="\$(CC)" ./configure --no-openssl \\
		--no-libxml2 --no-zlib --no-pcre --no-fs \\
		--ld-opt="$NGX_LD_OPT" \\
	&& \$(MAKE) libnjs libqjs

END
