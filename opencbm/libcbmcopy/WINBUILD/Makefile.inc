a65: ..\ppr-1541.inc ..\ppr-1571.inc ..\ppw-1541.inc ..\ppw-1571.inc ..\s1r-1581.inc ..\s1r.inc ..\s1w-1581.inc ..\s1w.inc ..\s2r-1581.inc ..\s2r.inc ..\s2w-1581.inc ..\s2w.inc ..\turboread1541.inc ..\turboread1571.inc ..\turboread1581.inc ..\turbowrite1541.inc ..\turbowrite1571.inc ..\turbowrite1581.inc

.SUFFIXES: .a65

{..\}.a65{..\}.inc:
    ..\..\buildoneinc ..\.. $?