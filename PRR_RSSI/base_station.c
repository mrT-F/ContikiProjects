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

#include "net/netstack.h"

#include <stdio.h>

static struct collect_conn tc;

/*---------------------------------------------------------------------------*/
PROCESS(base_station_process, "RSSI/PRR Base Station");
AUTOSTART_PROCESSES(&base_station_process);
/*---------------------------------------------------------------------------*/
static void
recv(const rimeaddr_t *originator, uint8_t seqno, uint8_t hops)
{
  static signed char rssi = cc2420_last_rssi+45;
  printf("Base Station got message from %d.%d, seqno %d, hops %d: len %d '%s'\n",
	 originator->u8[0], originator->u8[1],
	 seqno, hops,
	 packetbuf_datalen(),
	 (char *)packetbuf_dataptr());
  printf("\t RSSI Value: %d\n",rssi)

}
/*---------------------------------------------------------------------------*/
static const struct collect_callbacks callbacks = { recv };
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(base_station_process, ev, data)
{
  static struct etimer et;
  
  PROCESS_BEGIN();
//Open collect connection
  collect_open(&tc, 130, COLLECT_ROUTER, &callbacks);
//This is the base station, so this is the sink
	collect_set_sink(&tc, 1);


  /* Allow some time for the network to settle. */
  etimer_set(&et, 5 * CLOCK_SECOND);
  PROCESS_WAIT_UNTIL(etimer_expired(&et));

  while(1) {

    //Busy loop - I think we need this to keep receiving

  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/