#ifndef TODEVICE_HH
#define TODEVICE_HH

/*
=c

ToDevice(DEVNAME [, BURST, I<KEYWORDS>])

=s devices

sends packets to network device (kernel)

=d

This manual page describes the Linux kernel module version of the ToDevice
element. For the user-level element, read the ToDevice.u manual page.

Pulls packets from its single input and sends them out the Linux network
interface named DEVNAME. DEVNAME may also be an Ethernet address, in which
case ToDevice searches for a device with that address.

Sends up to BURST packets each time it is scheduled. By default, BURST is 16.
For good performance, you should set BURST to be 8 times the number of
elements that could generate packets for this device.

Packets must have a link header. For Ethernet, ToDevice makes sure every
packet is at least 60 bytes long.

Keyword arguments are:

=over 8

=item BURST

Unsigned integer. Same as the BURST argument.

=item ALLOW_NONEXISTENT

Allow nonexistent devices. If true, and no device named DEVNAME exists when
the router is initialized, then ToDevice will report a warning (rather than an
error). Later, while the router is running, if a device named DEVNAME appears,
ToDevice will seamlessly begin sending packets to it. Default is false.

=back

=n

The Linux networking code may also send packets out the device. Click won't
see those packets. Worse, Linux may cause the device to be busy when a
ToDevice wants to send a packet. Click is not clever enough to re-queue
such packets, and discards them. 

ToDevice interacts with Linux in two ways: when Click is running in polling
mode, or when Click is running in interrupt mode. In both of these cases,
we depend on the net driver's send operation for synchronization (e.g.
tulip send operation uses a bit lock).

=h packets read-only

Returns the number of packets ToDevice has pulled.

=h reset_counts write-only

Resets C<packets> counter to zero when written.

=a FromDevice, PollDevice, FromLinux, ToLinux, ToDevice.u */

#include "elements/bsdmodule/anydevice.hh"
#if 0
#include "elements/linuxmodule/fromlinux.hh"
#endif

class ToDevice : public AnyDevice {
  
 public:
  
  ToDevice();
  ~ToDevice();
  
  const char *class_name() const	{ return "ToDevice"; }
  const char *processing() const	{ return PULL; }
  ToDevice *clone() const		{ return new ToDevice; }
  
  int configure_phase() const		{ return CONFIGURE_PHASE_TODEVICE; }
  int configure(const Vector<String> &, ErrorHandler *);
  int initialize(ErrorHandler *);
  void uninitialize();
  void add_handlers();
  
  void run_scheduled();

  void reset_counts();
  void change_device(struct ifnet *);
  bool tx_intr();

#if CLICK_DEVICE_STATS
  // Statistics.
  unsigned long long _time_clean;
  unsigned long long _time_freeskb;
  unsigned long long _time_queue;
  unsigned long long _perfcnt1_pull;
  unsigned long long _perfcnt1_clean;
  unsigned long long _perfcnt1_freeskb;
  unsigned long long _perfcnt1_queue;
  unsigned long long _perfcnt2_pull;
  unsigned long long _perfcnt2_clean;
  unsigned long long _perfcnt2_freeskb;
  unsigned long long _perfcnt2_queue;
  unsigned long _activations; 
#endif
  unsigned _npackets;
#if CLICK_DEVICE_THESIS_STATS || CLICK_DEVICE_STATS
  unsigned long long _pull_cycles;
#endif
  unsigned long _rejected;
  unsigned long _hard_start;
  unsigned long _busy_returns;

#if HAVE_POLLING
  bool polling() const			{ return _dev && _dev->polling > 0; }
#else
  bool polling() const			{ return false; }
#endif
  
 private:

  unsigned _burst;
  int _dev_idle;
  
  int queue_packet(Packet *p);
  
};

#endif
