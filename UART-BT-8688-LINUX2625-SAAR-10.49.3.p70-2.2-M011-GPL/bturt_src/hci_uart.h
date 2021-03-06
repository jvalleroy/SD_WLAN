/*
 *
 *  Bluetooth HCI UART driver
 *
 *  Copyright (C) 2000-2001  Qualcomm Incorporated
 *  Copyright (C) 2002-2003  Maxim Krasnyansky <maxk@qualcomm.com>
 *  Copyright (C) 2004-2005  Marcel Holtmann <marcel@holtmann.org>
 *  Copyright (C) 2009, Marvell International Ltd.
 *
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
#include <linux/version.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,26)
#define TTY_FUNC tty->ops
#else
#define TTY_FUNC tty->driver
#endif

#ifndef N_HCI
#define N_HCI	15
#endif

/* Ioctls */
#define HCIUARTSETPROTO		_IOW('U', 200, int)
#define HCIUARTGETPROTO		_IOR('U', 201, int)
#define HCIUARTGETDEVICE	_IOR('U', 202, int)

/* UART protocols */
#define HCI_UART_MAX_PROTO	5

#define HCI_UART_H4	0
#define HCI_UART_BCSP	1
#define HCI_UART_3WIRE	2
#define HCI_UART_H4DS	3
#define HCI_UART_LL	4

struct hci_uart;

struct hci_uart_proto
{
    unsigned int id;
    int (*open) (struct hci_uart * hu);
    int (*close) (struct hci_uart * hu);
    int (*flush) (struct hci_uart * hu);
    int (*recv) (struct hci_uart * hu, void *data, int len);
    int (*enqueue) (struct hci_uart * hu, struct sk_buff * skb);
    struct sk_buff *(*dequeue) (struct hci_uart * hu);
};

struct hci_uart
{
    struct tty_struct *tty;
    struct hci_dev *hdev;
    unsigned long flags;

    struct hci_uart_proto *proto;
    void *priv;

    struct sk_buff *tx_skb;
    unsigned long tx_state;
    spinlock_t rx_lock;
};

/* HCI_UART flag bits */
#define HCI_UART_PROTO_SET	0

/* TX states  */
#define HCI_UART_SENDING	1
#define HCI_UART_TX_WAKEUP	2

int hci_uart_register_proto(struct hci_uart_proto *p);
int hci_uart_unregister_proto(struct hci_uart_proto *p);
int hci_uart_tx_wakeup(struct hci_uart *hu);

#ifdef CONFIG_BT_HCIUART_H4
int h4_init(void);
int h4_deinit(void);
#endif

#ifdef CONFIG_BT_HCIUART_BCSP
int bcsp_init(void);
int bcsp_deinit(void);
#endif

#ifdef CONFIG_BT_HCIUART_LL
int ll_init(void);
int ll_deinit(void);
#endif

#ifdef CONFIG_BT_HCIUART_PS
#define MRVL_ENTER_PS_CHAR      'D'
#define MRVL_EXIT_PS_CHAR       'W'
int proc_init(void);
void proc_remove(void);
void ps_init_timer(struct tty_struct *tty);
void ps_start_timer(void);
void ps_cancel_timer(void);
int ps_wakeup(void);
void ps_init(void);
void hexdump(char *prompt, u8 * buf, int len);
void ps_check_event_packet(struct sk_buff *skb);
void ps_send_char_complete(u8 ch);
#endif
