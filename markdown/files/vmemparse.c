#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static int vaheight = 0; /// used to put small VPNs at bottom, not top, of image
/// Call once before any draw_PNPage commands
void startImage(FILE *dst, unsigned char pages, unsigned char vpnbits, unsigned char processes) {
    // To learn more about any SVG element, see the MDN entry for it; for example the
    // <svg > element is documented at https://developer.mozilla.org/en-US/docs/Web/SVG/Element/svg
    vaheight = 1<<vpnbits;
    int paheight = 10*(pages/4)+10;
    int height = 21 + (vaheight > paheight? vaheight : paheight);
    fprintf(dst, "<svg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 %d %d' font-size='8' text-anchor='middle'>\n", 50*processes+40, height);
    
    // background gradient for unused memory
    fprintf(dst, "<defs>"
        "<linearGradient id='unused' x1='0' y1='0' x2='0.5%%' y2='3%%' spreadMethod='repeat'>"
        "<stop stop-color='#eee' offset='0%%'/>"
        "<stop stop-color='#ddd' offset='50%%'/>"
        "<stop stop-color='#eee' offset='100%%'/>"
        "</linearGradient>"
        "</defs>");
    
    // virtual memory outlines and labels
    vaheight = 1<<vpnbits;
    for(int i=0; i<processes; i+=1) {
        fprintf(dst, "<rect width='40' height='%d' x='%d' y='20' fill='url(#unused)'/>", 1<<vpnbits, 50*i+50);
        fprintf(dst, "<text x='%d' y='15'>virt %d</text>\n", 50*i+70, i+1);
    }
    // physical memory cells
    fprintf(dst, "<g id='ram' fill='none' stroke='black'>\n");
    for(int i=0; i<pages; i+=1) {
        fprintf(dst, "  <rect x='%d' y='%d' width='10' height='10'/>\n", 10*(i%4), 20+10*(i/4));
    }
    fprintf(dst, "</g>\n");
    fprintf(dst, "<text x='20' y='15'>RAM</text>\n");
}

/// Call once after all draw_PNPage commands
void endImage(FILE *dst) { fprintf(dst, "</svg>\n"); }

/// Adds a colored bar for a virtual page
void drawVPNPage(FILE *dst, unsigned process, unsigned page, int isKernel, int isCode, int isWriteable) {
    fprintf(dst, "<rect width='40' height='1.1' x='%d' y='%d' fill='#%s'/>\n",
         50+50*process, 20 + vaheight - page-1,
         isKernel ? (
            isWriteable ? (isCode ? "700" : "157")
            : (isCode ? "071" : "777")
         ) :
         isWriteable ? (isCode ? "f00" : "3af")
         : (isCode ? "0f3" : "fff")
    );
}

/// Adds a textual label to a physical page
void drawPPNPage(FILE *dst, unsigned page, char label) {
    fprintf(dst, "<text x='%d' y='%d'>%c</text>\n",
        10*(page%4)+5, 28+10*(page/4), label
    );
}

int main(int argc, char *argv[]) {
    if (argc != 2) { // verifiy argument count
        fprintf(stderr, "USAGE: %s filenametoparse\n", argv[0]);
        return 1;
    }
    FILE *fp = fopen(argv[1], "rb");
    if (!fp) { // verify that the argument was a file
        fprintf(stderr, "Cannot open \"%s\" for parsing\n", argv[1]);
        return 1;
    }
    
    // read the magic starting string
    char magic[11];
    if (11 != fread(magic, 1, 11, fp)) {
        fprintf(stderr, "Invalid file format: \"%s\" too short\n", argv[1]);
        return 1;
    }
    if (strncmp(magic, "3330 VMEM\n\0", 11)) {
        fprintf(stderr, "Invalid file format: \"%s\" missing magic number\n", argv[1]);
        return 1;
    }
    
    char oname[256]; snprintf(oname, 256, "%s.svg", argv[1]);
    FILE *dst = fopen(oname, "w");

    /////////////////////////
    // Your code goes here //
    /////////////////////////
    
    fclose(dst);    
    
    fclose(fp);
    return 0;
}
