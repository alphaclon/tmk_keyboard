#ifndef VIRTSER_H_
#define VIRTSER_H_

void virtser_init(void);
void virtser_recv(uint8_t c);
void virtser_task(void);
void virtser_send(const uint8_t byte);

#endif
