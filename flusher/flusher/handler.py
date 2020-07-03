import base64 as b64
from datetime import datetime
from sqlalchemy.dialects.postgresql import insert

from .db import (
    blocks,
    transactions,
    accounts,
    data_sources,
    oracle_scripts,
    requests,
    raw_requests,
    val_requests,
    reports,
    raw_reports,
    validators,
    delegations,
    account_transcations,
)


class Handler(object):
    def __init__(self, conn):
        self.conn = conn

    def handle_new_block(self, msg):
        self.conn.execute(blocks.insert(), msg)

    def handle_new_transaction(self, msg):
        related_tx_accounts = msg["account_transcations"]
        del msg["account_transcations"]
        res = self.conn.execute(transactions.insert(), msg)
        tx_id = res.inserted_primary_key[0]
        for account in related_tx_accounts:
            if account != "":
                self.conn.execute(account_transcations.insert(), {"id": tx_id, "address": account})

    def handle_set_account(self, msg):
        self.conn.execute(
            insert(accounts)
            .values(**msg)
            .on_conflict_do_update(constraint="accounts_pkey", set_=msg)
        )

    def handle_new_data_source(self, msg):
        self.conn.execute(data_sources.insert(), msg)

    def handle_new_oracle_script(self, msg):
        self.conn.execute(oracle_scripts.insert(), msg)

    def handle_new_request(self, msg):
        self.conn.execute(requests.insert(), msg)

    def handle_update_request(self, msg):
        condition = True
        for col in requests.primary_key.columns.values():
            condition = (col == msg[col.name]) & condition
        self.conn.execute(requests.update().where(condition).values(**msg))

    def handle_new_raw_request(self, msg):
        self.conn.execute(raw_requests.insert(), msg)

    def handle_new_val_request(self, msg):
        self.conn.execute(val_requests.insert(), msg)

    def handle_new_report(self, msg):
        self.conn.execute(reports.insert(), msg)

    def handle_new_raw_report(self, msg):
        self.conn.execute(raw_reports.insert(), msg)

    def handle_set_validator(self, msg):
        self.conn.execute(
            insert(validators)
            .values(**msg)
            .on_conflict_do_update(constraint="validators_pkey", set_=msg)
        )

    def handle_update_validator(self, msg):
        condition = True
        for col in validators.primary_key.columns.values():
            condition = (col == msg[col.name]) & condition
        self.conn.execute(validators.update().where(condition).values(**msg))

    def handle_set_delegation(self, msg):
        self.conn.execute(
            insert(delegations)
            .values(**msg)
            .on_conflict_do_update(constraint="delegations_pkey", set_=msg)
        )

    def handle_remove_delegation(self, msg):
        condition = True
        for col in delegations.primary_key.columns.values():
            condition = (col == msg[col.name]) & condition
        self.conn.execute(delegations.delete().where(condition))
