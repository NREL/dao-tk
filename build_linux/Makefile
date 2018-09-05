all:
	make -f Makefile-libclearsky -j4
	make -f Makefile-libcycle -j4
	make -f Makefile-liboptical -j4
	make -f Makefile-libsolar -j4
	make -f Makefile-libcluster -j4
	make -f Makefile-app -j4

clean:
	make -f Makefile-libclearsky clean
	make -f Makefile-libcycle clean
	make -f Makefile-liboptical clean
	make -f Makefile-libsolar clean
	make -f Makefile-libcluster clean
	make -f Makefile-app clean
