/*
 * Sender node for RSSI/PRR calculations
 * Based on example_collect.c
 */

#include "contiki.h"
#include "lib/random.h"
#include "net/rime.h"
#include "net/rime/collect.h"
#include "dev/leds.h"
#include "dev/button-sensor.h"
#include "dev/cc2420.h"
#include "net/netstack.h"

#include <stdio.h>

static struct collect_conn tc;

/*---------------------------------------------------------------------------*/
PROCESS(sender_node_process, "RSSI/PRR Sender Node");
AUTOSTART_PROCESSES(&sender_node_process);
/*---------------------------------------------------------------------------*/
static void
recv(const rimeaddr_t *originator, uint8_t seqno, uint8_t hops)
{
  printf("Sender node got message from %d.%d, seqno %d, hops %d: len %d '%s'\n",
	 originator->u8[0], originator->u8[1],
	 seqno, hops,
	 packetbuf_datalen(),
	 (char *)packetbuf_dataptr());
}
/*---------------------------------------------------------------------------*/
static const struct collect_callbacks callbacks = { recv };
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(sender_node_process, ev, data)
{
  static struct etimer periodic;
  static struct etimer et;
  static int pkts_sent;

  PROCESS_BEGIN();
  /*Open collect connection*/
  collect_open(&tc, 130, COLLECT_ROUTER, &callbacks);

  /* Allow some time for the network to settle. */
  etimer_set(&et, 5 * CLOCK_SECOND);
  PROCESS_WAIT_UNTIL(etimer_expired(&et));
  pkts_sent=0;
  while(1) {

    /* Send a packet every second until we send 200 */
    if(etimer_expired(&periodic)) {
      etimer_set(&periodic, CLOCK_SECOND * 2);
      etimer_set(&et, random_rand() % (CLOCK_SECOND * 2));
    }

    PROCESS_WAIT_EVENT();

    if(pkts_sent<20){
      if(etimer_expired(&et)) {
        static rimeaddr_t oldparent;
        const rimeaddr_t *parent;

        printf("Sending to Base Station\n");
        packetbuf_clear();
        packetbuf_set_datalen(sprintf(packetbuf_dataptr(),
  				  "%s", "Checking In") + 1);
        collect_send(&tc, 15);
        //Increment packets sent if successful
        pkts_sent=pkts_sent+1;
        printf("\tPackets Sent: %d\n",pkts_sent);
        printf("\tCollect transmissions: %d\n",tc.transmissions);
        parent = collect_parent(&tc);
        if(!rimeaddr_cmp(parent, &oldparent)) {
          if(!rimeaddr_cmp(&oldparent, &rimeaddr_null)) {
            printf("#L %d 0\n", oldparent.u8[0]);
          }
          if(!rimeaddr_cmp(parent, &rimeaddr_null)) {
            printf("#L %d 1\n", parent->u8[0]);
          }
          rimeaddr_copy(&oldparent, parent);
        }
      }
    }

  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/