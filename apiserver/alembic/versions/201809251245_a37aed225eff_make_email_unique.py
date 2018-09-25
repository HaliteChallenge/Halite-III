"""Make email unique

Revision ID: a37aed225eff
Revises: 39b7d45e4622
Create Date: 2018-09-25 12:45:45.826292+00:00

"""
from alembic import op
import sqlalchemy as sa
from sqlalchemy.dialects import mysql


# revision identifiers, used by Alembic.
revision = 'a37aed225eff'
down_revision = '39b7d45e4622'
branch_labels = None
depends_on = None


def upgrade():
    op.create_unique_constraint("uq_user_email", "user", ["email"])


def downgrade():
    op.drop_constraint("uq_user_email", "user")
