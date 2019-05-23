HPACK(3) - Library Functions Manual

# NAME

**hpack\_init**,
**hpack\_table\_new**,
**hpack\_table\_free**,
**hpack\_table\_size**,
**hpack\_decode**,
**hpack\_encode**,
**hpack\_header\_new**,
**hpack\_header\_add**,
**hpack\_header\_free**,
**hpack\_headerblock\_new**,
**hpack\_headerblock\_free**,
**hpack\_huffman\_decode**,
**hpack\_huffman\_decode\_str**,
**hpack\_huffman\_encode** - HPACK header compression for HTTP/2

# SYNOPSIS

**#include &lt;sys/queue.h>**  
**#include &lt;hpack.h>**

*int*  
**hpack\_init**(*void*);

*struct hpack\_table \*&zwnj;*  
**hpack\_table\_new**(*size\_t max\_table\_size*);

*void*  
**hpack\_table\_free**(*struct hpack\_table \*hpack*);

*size\_t*  
**hpack\_table\_size**(*struct hpack\_table \*hpack*);

*struct hpack\_headerblock \*&zwnj;*  
**hpack\_decode**(*unsigned char \*data*, *size\_t len*, *struct hpack\_table \*hpack*);

*unsigned char \*&zwnj;*  
**hpack\_encode**(*struct hpack\_headerblock \*hdrs*, *size\_t \*encoded\_len*, *struct hpack\_table \*hpack*);

*struct hpack\_header \*&zwnj;*  
**hpack\_header\_new**(*void*);

*struct hpack\_header \*&zwnj;*  
**hpack\_header\_add**(*struct hpack\_headerblock \*hdrs*, *const char \*key*, *const char \*value*, *enum hpack\_header\_index index*);

*void*  
**hpack\_header\_free**(*struct hpack\_header \*hdr*);

*struct hpack\_headerblock \*&zwnj;*  
**hpack\_headerblock\_new**(*void*);

*void*  
**hpack\_headerblock\_free**(*struct hpack\_headerblock \*hdrs*);

*unsigned char \*&zwnj;*  
**hpack\_huffman\_decode**(*unsigned char \*data*, *size\_t len*, *size\_t \*decoded\_len*);

*char \*&zwnj;*  
**hpack\_huffman\_decode\_str**(*unsigned char \*data*, *size\_t len*);

*unsigned char \*&zwnj;*  
**hpack\_huffman\_encode**(*unsigned char \*data*, *size\_t len*, *size\_t \*encoded\_len*);

# DESCRIPTION

The
**hpack**
family of functions provides an API to decode and encode HPACK header
compression for HTTP/2.

The
*hpack\_header*
and
*hpack\_headerblock*
structures are defined as
&lt;*sys/queue.h*>
tail queue element and head of the following types:

	struct hpack_header {
		char				*hdr_name;
		char				*hdr_value;
		enum hpack_header_index		 hdr_index;
		TAILQ_ENTRY(hpack_header)	 hdr_entry;
	};
	TAILQ_HEAD(hpack_headerblock, hpack_header);

The
*index*
argument can be specified as one of the following values:
`HPACK_INDEX`,
`HPACK_NO_INDEX`,
or
`HPACK_NEVER_INDEX`
to add the header to the index of the dynamic HPACK table,
to exclude the header from the index,
or to exclude the header from the index and to mark it as sensitive to
never include it in the index.

# RETURN VALUES

**hpack\_init**()
returns 0 on success or -1 on error.

**hpack\_table\_size**()
returns the current size of the dynamic HPACK table or 0 if it is empty.

**hpack\_table\_new**(),
**hpack\_decode**(),
**hpack\_encode**(),
**hpack\_header\_new**(),
**hpack\_header\_add**(),
**hpack\_headerblock\_new**(),
**hpack\_huffman\_decode**(),
**hpack\_huffman\_decode\_str**(),
and
**hpack\_huffman\_encode**()
return
`NULL`
on error or an out-of-memory condition.

# SEE ALSO

queue(3),
relayd(8)

# STANDARDS

R. Peon,
H. Ruellan,
*HPACK: Header Compression for HTTP/2*,
RFC 7541,
May 2015.

# HISTORY

The
**hpack**
API first appeared on
[https://github.com/reyk/hpack](https://github.com/reyk/hpack).

# AUTHORS

Reyk Floeter &lt;[reyk@openbsd.org](mailto:reyk@openbsd.org)&gt;

OpenBSD 6.5 - May 23, 2019
