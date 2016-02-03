/*
* Base Station for RSSI/PRR Calculations
* Based on example-collect.c 
*
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

static int numberPacketsReceived;
static int avgRSSI;
static int totalRSSI;
static struct broadcast_conn broadcast;

/*---------------------------------------------------------------------------*/
PROCESS(base_station_process, "RSSI/PRR Base Station");
AUTOSTART_PROCESSES(&base_station_process);
/*---------------------------------------------------------------------------*/
static void
recv(const rimeaddr_t *originator, uint8_t seqno, uint8_t hops)
{
  static signed char rssi;
  rssi = cc2420_last_rssi+45;
  totalRSSI += rssi;
  printf("Base Station got message from %d.%d, seqno %d, hops %d: len %d '%s'\n",
	 originator->u8[0], originator->u8[1],
	 seqno, hops,
	 packetbuf_datalen(),
	 (char *)packetbuf_dataptr());
  printf("\t RSSI Value: %d\n",rssi);
  printf("\t Average RSSI: %d\n",avgRSSI);
  printf("Total packets received: %d\n",++numberPacketsReceived);
  avgRSSI = (totalRSSI) / numberPacketsReceived;
  
}
/*---------------------------------------------------------------------------*/
static const struct broadcast_callbacks broadcast_call = { recv };
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(base_station_process, ev, data)
{
  static struct etimer et;
  
  PROCESS_BEGIN();

  numberPacketsReceived = 0;
  broadcast_open(&broadcast, 129, &broadcast_call);

  /* Allow some time for the network to settle. */
  etimer_set(&et, 30 * CLOCK_SECOND);
  PROCESS_WAIT_UNTIL(etimer_expired(&et));

  while(1) {

    //Busy loop - I think we need this to keep receiving
    if(etimer_expired(&et)) {
      etimer_set(&et, random_rand() % (CLOCK_SECOND * 30));
    }
    PROCESS_WAIT_EVENT();
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/